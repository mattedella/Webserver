
#include "../includes/Request.hpp"
#include <cstdio>
#include <sstream>
#include <string>
#include <utility>

Request::Request() {}

void Request::ParsRequest(std::string& Request) {
	std::stringstream ss(Request);
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

void Request::printRequest() {
	std::cout << "Method: " + _method + '\n';
	std::cout << "URL: " + _url + '\n';
	std::cout << "HTTP Version: " + _httpVersion + '\n';
	std::cout << "Headers: \n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first + ": " + it->second + '\n';
}

Request::~Request() {}