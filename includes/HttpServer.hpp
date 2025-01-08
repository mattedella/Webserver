#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

class HTTPServer {
private:
    struct ServerSocket {
        int fd;
        int port;
    };

    struct Route {
        std::string content;
        std::string content_type;
    };

    std::vector<ServerSocket> server_sockets;
    std::vector<struct pollfd> poll_fds;
    std::map<int, std::string> client_buffers;
    std::map<int, std::string> client_responses;
    std::map<std::string, Route> routes;

    static const int MAX_CLIENTS = 10;
    static const int BUFFER_SIZE = 1024;
    static const size_t MAX_BODY_SIZE = 1000000;

    void setup_routes();
    void setup_error_pages();
    void set_nonblocking(int fd);
    std::string create_response(int status_code, const std::string& content_type, const std::string& body);
    void handle_new_connection(int server_fd);
    std::string extract_path(const std::string& request);
    std::string extract_method(const std::string& request);
    void handle_request(int client_fd, const std::string& request);
    void handle_client_data(int index);
    void handle_client_response(int index);
    void close_connection(int index);

public:
    HTTPServer();
    ~HTTPServer();
    bool init(int port);
    void run();
};

#endif
