#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstring> 
#include "../includes/webserv.hpp"

void server::close_connection(int index) {
    if (index < 0 || (int)getPollFds().size()) {
        std::cerr << "Invalid poll_fds index" << std::endl;
        return;
    }
    int client_fd = _poll_fds[index].fd;

    if(client_fd > 0)
    {
        close(client_fd);
        _client_buffers.erase(client_fd);
        _client_responses.erase(client_fd);
    }
    if (index < (int)getPollFds().size())
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


void server::handle_client_response(int index) {
    if (index < 0 || index >= (int)getPollFds().size()) {
        std::cerr << "Invalid poll_fds index" << std::endl;
        return;
    }
    int client_fd = _poll_fds[index].fd;
    if (_client_responses.find(client_fd) == _client_responses.end()) {
        close_connection(index);
        return;
    }
    const std::string& response = _client_responses[client_fd];
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0 || static_cast<size_t>(bytes_sent) != response.length()) {
        std::cerr << "Send error or partial send" << std::endl;
        close_connection(index);
        return;
    }

    close_connection(index);
}

void server::handle_request(int client_fd, const std::string& request) {
    std::istringstream req_stream(request);
    std::string line;
    std::string host;


    std::cout<< request <<"\n";
    while (std::getline(req_stream, line)) {
        if (line.find("Host:") == 0) {
            host = line.substr(6); // Extract hostname (remove "Host: ")
            size_t start = host.find_first_not_of(" \t");
            if (start != std::string::npos) {
                host = host.substr(start);
            }
            size_t end = host.find("\r");
            if (end != std::string::npos) {
                host = host.substr(0, end); // Remove trailing CRLF
            }
            break;
        }
        if (line == "\r") { // Stop parsing at the end of headers
            break;
        }
    }

    bool matched = false;
    for (std::vector<std::string>::const_iterator it = _server_names.begin(); it != _server_names.end(); ++it) {
        if (*it == host) {
            matched = true;
            break;
        }
    }
    if (!matched) {
        _client_responses[client_fd] = "HTTP/1.1 404 Not Found\r\n\r\n";
        return;
    }
    _client_responses[client_fd] = "HTTP/1.1 200 OK\r\n\r\nHello, " + host + "!";
}




void server::handle_client_data(int index) {
    if (index < 0 || index >= (int)getPollFds().size()) {
        std::cerr << "Invalid poll_fds index" << std::endl;
        return;
    }

    int client_fd = _poll_fds[index].fd;

    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        close_connection(index);
        return;
    }
    buffer[bytes_read] = '\0';
    _client_buffers[client_fd].append(buffer, bytes_read);
    if (_client_buffers[client_fd].find("\r\n\r\n") != std::string::npos) {
        handle_request(client_fd, _client_buffers[client_fd]);
        if (index < (int)getPollFds().size()) {
            _poll_fds[index].events = POLLOUT;
        }
    }
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
}

void server::starting()
{


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

void server::s_run(conf ConfBlock, Request* req)
{
    // Single iteration of event handling
    int ret = poll(&_poll_fds[0], _poll_fds.size(), 0); // Non-blocking poll
    
    if (ret < 0) {
        std::cerr << "Poll error: " << strerror(errno) << std::endl;
        return;
    }

    if (ret > 0) {
        for (size_t i = 0; i < _poll_fds.size(); ++i) {
            if (_poll_fds[i].revents == 0)
                continue;

            try {
                bool is_server_socket = false;
                for (size_t j = 0; j < _server_sockets.size(); ++j) {
                    if (_poll_fds[i].fd == _server_sockets[j].fd) {
                        is_server_socket = true;
                        if (_poll_fds[i].revents & POLLIN) {
                            handle_new_connection(_server_sockets[j].fd);
                        }
                        break;
                    }
                }
                if (!is_server_socket) {
                    if (_poll_fds[i].revents & POLLIN) {
                        std::cout << "Request on server port " << getPort() << std::endl;
                        req->getRequest(_poll_fds[i].fd, _poll_fds[i].events);
                        if (req->isComplete()) {
                            _poll_fds[i].events = POLLOUT;
                        }
                    }
                    if (_poll_fds[i].events & POLLOUT) {
                        std::cout << "Response from server port " << getPort() << std::endl;
                        sendResponse(_poll_fds[i].fd, ConfBlock, req);
                        req->clear();
                        _poll_fds[i].events = POLLIN;
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