#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <vector>
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

bool	server::init(int port)
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) return false;
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
	{
		close(server_fd);
		return false;
	}
	set_nonblocking(server_fd);
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		close(server_fd);
		return false;
	}
	if (listen(server_fd, 3) < 0) {
		close(server_fd);
		return false;
	}
	ServerSocket ss;
	ss.fd = server_fd;
	ss.port = port;
	_server_sockets.push_back(ss);
	struct pollfd pfd;
	pfd.fd = server_fd;
	pfd.events = POLLIN;
	_poll_fds.push_back(pfd);

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

void server::s_run(conf* ConfBlock, Request* req)
{
	for (size_t i = 0; i < _poll_fds.size(); ++i) {

		int bo = poll(&_poll_fds[i], _poll_fds.size(), -1);
		
		if (Quit == 1)
			return;
		std::cout << "bo: "<<bo <<" poll fds data: "<< _poll_fds.data() << " poll fd siz : "<< _poll_fds.size() << "\n";
		std::cout <<"fd i "<< _poll_fds[i].fd << "i = "<< i <<"\n";
		std::cout<< "revents " << _poll_fds[i].revents << "\n";
		if (bo <= 0)
			throw exc ("ci pensiamo dopo\n");

		
		if (_poll_fds[i].revents == 0)
			continue;
		if (_poll_fds[i].revents == 8 && _poll_fds[i].revents == 16 && _poll_fds[i].revents == 32)
			throw exc(" 8 16 32\n");
		try {
			if (_poll_fds[i].fd == _server_sockets[0].fd) {
				if (_poll_fds[i].revents & POLLIN) {
					handle_new_connection(_server_sockets[0].fd);
				}
			} 
			else {
				if (_poll_fds[i].revents & POLLIN) {
					req->getRequest(_poll_fds[i].fd, _poll_fds[i].events, _bodysize * 1000000, ConfBlock);
					Request();
				}
				if (_poll_fds[i].revents & POLLOUT) {
					sendResponse(_poll_fds[i].fd, ConfBlock, req, _poll_fds[i].events);
					req->clear();
					close_connection(i);
					break ;
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			close_connection(i);
		}
	}
}