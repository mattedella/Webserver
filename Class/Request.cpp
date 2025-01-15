
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

	std::getline(ss, line);
	std::stringstream req_line(line);
	req_line >> _method >> _url >> _httpVersion;
	while (std::getline(ss, line) && !line.empty()) {
		size_t colon_pos = line.find(':');
		if (colon_pos != NOT_FOUND) {
			std::string Key = line.substr(0, colon_pos);
			std::string Tp = line.substr(colon_pos + 2);
			_headers.insert(std::make_pair(Key, Tp));
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


void Request::printRequest() {
	std::cout << "Method: " + _method + '\n';
	std::cout << "URL: " + _url + '\n';
	std::cout << "HTTP Version: " + _httpVersion + '\n';
	std::cout << "Headers: \n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first + ": " + it->second + '\n';
}

std::string Request::getHeader(const std::string& Key) {
	return _headers[Key];
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