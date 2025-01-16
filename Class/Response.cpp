
#include "../includes/webserv.hpp"
#include <fstream>
#include <sstream>

Response::Response() {}

void Response::generateResponse(Request* req, conf ConfBlock) {
	
	std::ifstream file;
	std::stringstream buff;
	std::string request;
	std::cout << "Status Code: " << StatusCode << '\n';
	switch (StatusCode) {
    case 200:
        file.open(ConfBlock.getFullPath().c_str());
        buff << file.rdbuf();
        request = buff.str();
        _response = "HTTP/1.1 200 OK\r\nContent-Type: " + req->getHeader("Content-Type") + "\r\n"
                + "Connection: keep-alive\r\n"
                + "Content-Length: " + sizeToString(request.length()) + "\r\n"
                + "\r\n"
                + request;
        break;

    case 404:
        file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(404, 1, ConfBlock.getLocation(req->getURL(), 1))).c_str());
        buff << file.rdbuf();
        request = buff.str();
        _response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: "
                + sizeToString(request.length()) + "\r\n"
                + "\r\n"
                + request;
        break;

    case 403:
        file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(403, 1, ConfBlock.getLocation(req->getURL(),1 ))).c_str());
        buff << file.rdbuf();
        request = buff.str();
        _response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: "
                + sizeToString(request.length()) + "\r\n"
                + "\r\n"
                + request;
        break;

    case 408:
        file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(408, 1, ConfBlock.getLocation(req->getURL(), 1))).c_str());
        buff << file.rdbuf();
        request = buff.str();
        _response = "HTTP/1.1 408 Request Timeout\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Length: "
                + sizeToString(request.length()) + "\r\n"
                + "\r\n"
                + request;
        break;

    case 501:
        file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(501, 1, ConfBlock.getLocation(req->getURL(), 1))).c_str());
        buff << file.rdbuf();
        request = buff.str();
        _response = "HTTP/1.1 501 Method Not Allowed\r\n Connection: keep-alive\r\nContent-Length: "
                + sizeToString(request.length()) + "\r\n"
                + "\r\n"
                + request;
        break;
}
}

std::string Response::sizeToString(size_t size) {
    std::stringstream ss;
    ss << size;
    return ss.str();
}

std::string Response::getResponse() {
	return _response;
}

Response::~Response() {}