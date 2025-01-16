
#include "../includes/Request.hpp"
#include "../includes/Exc.hpp"
#include <cstdio>
#include <sstream>
#include <string>
#include <utility>
#include <poll.h>

Request::Request() {}

void Request::ParsRequest(char* Request) {
    std::string to_pars(Request);
    std::stringstream ss(to_pars);
    std::string line;

    // Parse request line
    std::getline(ss, line);
    std::stringstream req_line(line);
    req_line >> _method >> _url >> _httpVersion;

    // Parse headers
    bool headers_ended = false;
    while (std::getline(ss, line)) {
        // Check for end of headers
        if (line == "\r" || line.empty()) {
            headers_ended = true;
            break;
        }

        size_t colon_pos = line.find(':');
        if (colon_pos != NOT_FOUND) {
            std::string Key = line.substr(0, colon_pos);
            std::string Tp = line.substr(colon_pos + 2);
            _headers.insert(std::make_pair(Key, Tp));
        }
    }

    // Set headers complete if we found end marker
    if (headers_ended) {
        setHeadersComplete(true);
        
        // Handle POST requests
        if (_method == "POST") {
            std::string length = getHeader("Content-Length");
            if (!length.empty()) {
                setContentLength(std::atoi(length.c_str()));
                
                // Parse body if present
                std::string body;
                while (std::getline(ss, line)) {
                    body += line + "\n";
                }
                if (body.length() >= _content_length) {
                    setBodyComplete(true);
                }
            }
        }
    }
}

void Request::getRequest(int &client_socket, short& event) {
	int bytes_recived;
	char buffer[4096];

	bytes_recived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_recived < 0) {
		throw exc("Error reading request\n");
	}
	buffer[bytes_recived] = '\0';
	ParsRequest(buffer);
	printRequest();
	event = POLLOUT;
}

bool Request::isComplete() const {
    // Check request line components
    if (_method.empty() || _url.empty() || _httpVersion.empty())
        return false;

    // Check headers completion
    if (!_headers_complete)
        return false;

    // Check POST request body
    if (_method == "POST") {
        if (!_body_complete || _content_length == 0)
            return false;
    }

    return true;
}

void Request::setHeadersComplete(bool complete) {
    _headers_complete = complete;
}

void Request::setBodyComplete(bool complete) {
    _body_complete = complete;
}

void Request::setContentLength(size_t length) {
    _content_length = length;
}

void Request::clear() {
    _method.clear();
    _url.clear();
    _httpVersion.clear();
    _headers.clear();
    _headers_complete = false;
    _body_complete = false;
    _content_length = 0;
}

void Request::printRequest() {
	std::cout << "Method: " + _method + '\n';
	std::cout << "URL: " + _url + '\n';
	std::cout << "HTTP Version: " + _httpVersion + '\n';
	std::cout << "Headers: \n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first + ": " + it->second + '\n';
}

std::string Request::getHeader(const std::string& Key) {
    std::map<std::string, std::string>::iterator it = _headers.find(Key);
    if (it != _headers.end()) {
        return it->second;
    }
    
    std::cout << "!!!!!!!!!!!!!!!!!!!!"<< _url << '\n';
    if (Key == "Content-Type") {
        if (_url.find(".html") != std::string::npos)
            return "text/html";
        else if (_url.find(".css") != std::string::npos)
            return "text/css";
        else if (_url.find(".js") != std::string::npos)
            return "application/javascript";
        else if (_url.find(".png") != std::string::npos)
            return "image/png";
        else if (_url.find(".jpg") != std::string::npos || _url.find(".jpeg") != std::string::npos)
            return "image/jpeg";
        else
            return "text/plain";
    }
    
    return "";
}

std::string Request::getHttpVersion() {
	return _httpVersion;
}

std::string Request::getMethod() {
	return _method;
}

std::string& Request::getURL() {
	return _url;
}

Request::~Request() {}