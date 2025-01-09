#include "../includes/Server.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

void server::handle_request(int client_fd, const std::string& request) {
    std::string method = extract_method(request);
    std::string path = extract_path(request);
    if (path.empty() || path == "/") {
        path = "/";
    }
    if (method != "GET" && method != "POST" && method != "DELETE") {
        _client_responses[client_fd] = create_response(405, routes["/405"].content_type, routes["/405"].content);
        return;
    }
    if (method == "POST" && path == "/submit") {
        Route response;
        response.content_type = "text/html";
        response.content = "<html><body><h1>Form Submitted</h1><p>Thank you for your submission!</p>"
                           "<a href='/'>Back to home</a></body></html>";
        _client_responses[client_fd] = create_response(200, response.content_type, response.content);
        return;
    }
    if (method == "DELETE") {
        Route response;
        response.content_type = "text/html";
        response.content = "<html><body><h1>Delete Request Received</h1>"
                           "<p>Resource deletion simulated.</p></body></html>";
        _client_responses[client_fd] = create_response(200, response.content_type, response.content);
        return;
    }
    if (routes.find(path) != routes.end()) {
        const Route& route = routes[path];
        _client_responses[client_fd] = create_response(200, route.content_type, route.content);
    } else {
        _client_responses[client_fd] = create_response(404, routes["/404"].content_type, routes["/404"].content);
    }
}

void server::set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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


void server::handle_client_response(int index) {
    int client_fd = _poll_fds[index].fd;
    if (_client_responses.find(client_fd) == _client_responses.end()) {
        close_connection(index);
        return;
    }
    const std::string& response = _client_responses[client_fd];
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    if (bytes_sent <= 0) {
        close_connection(index);
        return;
    }
    close_connection(index);
}

void server::handle_client_data(int index) {
    char buffer[BUFFER_SIZE];
    int client_fd = _poll_fds[index].fd;
    ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        close_connection(index);
        return;
    }
    buffer[bytes_read] = '\0';
    _client_buffers[client_fd] += buffer;
    if (_client_buffers[client_fd].find("\r\n\r\n") != std::string::npos) {
        handle_request(client_fd, _client_buffers[client_fd]);
        _poll_fds[index].events = POLLOUT;
    }
}

void server::run() {
    std::cout << "Server running on port " << _server_sockets[0].port << std::endl;
    std::cout << "Available routes:" << std::endl;

    if (!_locations.empty())
		for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++)
			std::cout<<it->second.getRoot() + it->first+"\n";

	while (true) {
        int poll_count = poll(&_poll_fds[0], _poll_fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll failed" << std::endl;
            continue;
        }
        for (size_t i = 0; i < _poll_fds.size(); ++i) {
            if (_poll_fds[i].revents == 0)
                continue;
            if (_poll_fds[i].fd == _server_sockets[0].fd) {
                handle_new_connection(_server_sockets[0].fd);
            } else {
                if (_poll_fds[i].revents & POLLIN) {
                    handle_client_data(i);
                } else if (_poll_fds[i].revents & POLLOUT) {
                    handle_client_response(i);
                }
            }
        }
    }
}

bool	server::init()
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) return false;

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
	{
		close(server_fd);
		return false;
	}
}