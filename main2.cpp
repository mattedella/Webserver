#include "HTTPServer.hpp"

void HTTPServer::setup_routes() {
    Route home;
    home.content_type = "text/html";
    home.content = "<html><body>"
                   "<h1>Welcome to WebServ</h1>"
                   "<p>Funziona</p>"
                   "</body></html>";
    routes["/"] = home;
}

void HTTPServer::setup_error_pages() {
    Route error400;
    error400.content_type = "text/html";
    error400.content = "<html><body><h1>400 Bad Request</h1></body></html>";
    routes["/400"] = error400;

    Route error404;
    error404.content_type = "text/html";
    error404.content = "<html><body><h1>404 Not Found</h1></body></html>";
    routes["/404"] = error404;

    Route error405;
    error405.content_type = "text/html";
    error405.content = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
    routes["/405"] = error405;
}

void HTTPServer::set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::string HTTPServer::create_response(int status_code, const std::string& content_type, const std::string& body) {
    std::stringstream response;
    response << "HTTP/1.1 " << status_code << " ";
    switch (status_code) {
        case 200: response << "OK"; break;
        case 400: response << "Bad Request"; break;
        case 404: response << "Not Found"; break;
        case 405: response << "Method Not Allowed"; break;
        default: response << "Internal Server Error";
    }
    response << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;
    return response.str();
}

void HTTPServer::handle_new_connection(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) return;
    set_nonblocking(client_fd);
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    poll_fds.push_back(pfd);
    client_buffers[client_fd] = "";
}

std::string HTTPServer::extract_path(const std::string& request) {
    std::istringstream iss(request);
    std::string method, path, protocol;
    iss >> method >> path >> protocol;
    return path;
}

std::string HTTPServer::extract_method(const std::string& request) {
    std::istringstream iss(request);
    std::string method;
    iss >> method;
    return method;
}

void HTTPServer::handle_request(int client_fd, const std::string& request) {
    std::string method = extract_method(request);
    std::string path = extract_path(request);
    if (path.empty() || path == "/") {
        path = "/";
    }
    if (method != "GET" && method != "POST" && method != "DELETE") {
        client_responses[client_fd] = create_response(405, routes["/405"].content_type, routes["/405"].content);
        return;
    }
    if (method == "POST" && path == "/submit") {
        Route response;
        response.content_type = "text/html";
        response.content = "<html><body><h1>Form Submitted</h1><p>Thank you for your submission!</p>"
                           "<a href='/'>Back to home</a></body></html>";
        client_responses[client_fd] = create_response(200, response.content_type, response.content);
        return;
    }
    if (method == "DELETE") {
        Route response;
        response.content_type = "text/html";
        response.content = "<html><body><h1>Delete Request Received</h1>"
                           "<p>Resource deletion simulated.</p></body></html>";
        client_responses[client_fd] = create_response(200, response.content_type, response.content);
        return;
    }
    if (routes.find(path) != routes.end()) {
        const Route& route = routes[path];
        client_responses[client_fd] = create_response(200, route.content_type, route.content);
    } else {
        client_responses[client_fd] = create_response(404, routes["/404"].content_type, routes["/404"].content);
    }
}

void HTTPServer::handle_client_data(int index) {
    char buffer[BUFFER_SIZE];
    int client_fd = poll_fds[index].fd;
    ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        close_connection(index);
        return;
    }
    buffer[bytes_read] = '\0';
    client_buffers[client_fd] += buffer;
    if (client_buffers[client_fd].find("\r\n\r\n") != std::string::npos) {
        handle_request(client_fd, client_buffers[client_fd]);
        poll_fds[index].events = POLLOUT;
    }
}

void HTTPServer::handle_client_response(int index) {
    int client_fd = poll_fds[index].fd;
    if (client_responses.find(client_fd) == client_responses.end()) {
        close_connection(index);
        return;
    }
    const std::string& response = client_responses[client_fd];
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    if (bytes_sent <= 0) {
        close_connection(index);
        return;
    }
    close_connection(index);
}

void HTTPServer::close_connection(int index) {
    int client_fd = poll_fds[index].fd;
    close(client_fd);
    client_buffers.erase(client_fd);
    client_responses.erase(client_fd);
    poll_fds.erase(poll_fds.begin() + index);
}

HTTPServer::HTTPServer() {
    setup_routes();
    setup_error_pages();
}

HTTPServer::~HTTPServer() {
    for (size_t i = 0; i < server_sockets.size(); ++i) {
        if (server_sockets[i].fd >= 0)
            close(server_sockets[i].fd);
    }
}

bool HTTPServer::init(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return false;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
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
    server_sockets.push_back(ss);
    struct pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN;
    poll_fds.push_back(pfd);
    return true;
}

void HTTPServer::run() {
    std::cout << "Server running on port " << server_sockets[0].port << std::endl;
    std::cout << "Available routes:" << std::endl;
    std::cout << "  - GET  /" << std::endl;
    std::cout << "  - POST /submit" << std::endl;
    while (true) {
        int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll failed" << std::endl;
            continue;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents == 0)
                continue;
            if (poll_fds[i].fd == server_sockets[0].fd) {
                handle_new_connection(server_sockets[0].fd);
            } else {
                if (poll_fds[i].revents & POLLIN) {
                    handle_client_data(i);
                } else if (poll_fds[i].revents & POLLOUT) {
                    handle_client_response(i);
                }
            }
        }
    }
}