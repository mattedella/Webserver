#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/types.h>
#include <vector>
#include <cstring> 
#include "../includes/webserv.hpp"

void server::closeSocket() {
	for (std::vector<struct pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); it++)
		close(it->fd);
}

void server::addNametoHost() {
	std::ofstream		hosts;
	std::ofstream		checkhosts;
	std::stringstream	checker;
	std::string			to_check;
	hosts.open("/etc/hosts", std::ios::app);
	checkhosts.open("/etc/hosts", std::ios::in);
	if (!hosts.is_open()) {
		std::cout << "ERROR: cannot open \"/etc/hosts\"\n";
		return ;
	}
	checker << checkhosts.rdbuf();
	to_check = checker.str();
	std::cout << to_check << std::endl;
	for (std::vector<std::string>::iterator it = _server_names.begin(); it != _server_names.end(); it++) {
		std::string to_add = "127.0.1.1 " + *it;
		if (to_check.find(to_add) != NOT_FOUND)
			continue ;
		to_add += '\n';
		hosts.write(to_add.c_str(), to_add.size());
	}
	hosts.close();
}

void server::close_connection(int index) {
	int client_fd = _poll_fds[index].fd;
	close(client_fd);
	_client_buffers.erase(client_fd);
	_client_responses.erase(client_fd);
	_poll_fds.erase(_poll_fds.begin() + index);
}

void server::handle_new_connection(int server_fd)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
	if (client_fd < 0) return;
	set_nonblocking(client_fd);
	struct pollfd pfd;
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	_poll_fds.push_back(pfd);
	_client_buffers[client_fd] = "";
}

void server::set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool server::init(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
        return false;

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        close(server_fd);
        return false;
    }

    set_nonblocking(server_fd);
    
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        return false;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {  // Use SOMAXCONN instead of 3
        close(server_fd);
        return false;
    }

    // Add to server sockets array
    ServerSocket ss;
    ss.fd = server_fd;
    ss.port = port;
    _server_sockets.push_back(ss);

    // Add to poll array
    struct pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _poll_fds.push_back(pfd);

    return true;
	return true;
}

void server::starting() {
	for (size_t i = 0; i < _server_sockets.size(); i++)
		std::cout << "Server running on port " << GREEN <<_server_sockets[i].port << RESET <<std::endl;
	
	std::cout << "Available routes:" << std::endl;

	if (!_locations.empty())
	{
		for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++)
		{
			std::cout<< YELLOW <<it->second.getRoot() + it->first+"\n" << RESET;
		}
	}
}

void server::printFdsVect()
{
	std::vector<struct pollfd>::iterator it = _poll_fds.begin();
	for(; it != _poll_fds.end(); it++ )
	{
		std::cout << it->fd << std::endl;
	}
}

int server::getPort() const 
{
    if (!_server_sockets.empty()) {
        return _server_sockets[0].port;
    }
    return -1;
}

void server::s_run(conf* ConfBlock, Request* req)
{
	// Single iteration of event handling
	int ret = poll(&_poll_fds[0], _poll_fds.size(), 0); // Non-blocking poll
	bool is_server_socket = false;

	if (Quit == true)
		return;
	if (ret < 0) {
		std::cerr << "Poll error: " << strerror(errno) << std::endl;
		return;
	}

	if (ret > 0) {
		for (size_t i = 0; i < _poll_fds.size(); ++i) {
			// ! ci siamo quasi, la pagina viene caricata sul client ma non riceve mai la risposta dio madonna
			ret = poll(&_poll_fds[i], _poll_fds.size(), 0); // Non-blocking poll
			if (ret < 0) {
				std::cerr << "Poll error: " << strerror(errno) << std::endl;
				return;
			}
			if (_poll_fds[i].revents == 0)
				continue;

			try {
				for (size_t j = 0; j < _server_sockets.size(); ++j) {
					if (_poll_fds[i].fd == _server_sockets[j].fd) {
						is_server_socket = true;
						if (_poll_fds[i].revents == POLLIN) {
							handle_new_connection(_server_sockets[j].fd);
							std::cout << "Connection created\n";
						}
						break;
					}
				}
				if (!is_server_socket) {
					if (_poll_fds[i].revents & POLLIN) {
						std::cout << "Request on server port " << getPort() << std::endl;
						req->getRequest(_poll_fds[i].fd, _poll_fds[i].events, _bodysize * 1000000, ConfBlock);
						_poll_fds[i].events = POLLOUT; // Set for writing response
					}

					if (_poll_fds[i].revents & POLLOUT) {
						std::cout << "Response from server port " << getPort() << std::endl;
						bool finish = sendResponse(_poll_fds[i].fd, ConfBlock, req, _poll_fds[i].events);
						req->clear();  // Clear the request after sending the response
						if (finish == true) {
							_poll_fds[i].events = 0;
						} else {
							_poll_fds[i].events = POLLIN; // Re-enable POLLIN for the next request
						}
					}
				}
			} catch (const std::exception& e) {
				std::cerr << "Error on fd " << _poll_fds[i].fd << ": " << e.what() << std::endl;
				req->clear();
				close_connection(i);
			}
			_poll_fds[i].revents = 0;
		}
	}
}
