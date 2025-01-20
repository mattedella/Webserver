
#include "../includes/webserv.hpp"
#include <cstdio>
#include <ios>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <utility>
#include <poll.h>

Request::Request() {}

void Request::clear() {
	_method.clear();
	_headers.clear();
	_file.clear();
	_httpVersion.clear();
	_url.clear();
	_body.clear();
}

void Request::parsGet(std::stringstream& file, std::string& line) {
	while (std::getline(file, line) && !line.empty()) {
			size_t colon_pos = line.find(':');
			if (colon_pos != NOT_FOUND) {
				std::string Key = line.substr(0, colon_pos);
				std::string Tp = line.substr(colon_pos + 2);
				_headers.insert(std::make_pair(Key, Tp));
			}
		}
}

void Request::parsMultipart(std::stringstream& file, std::string& line, std::string Type) {
	std::string Key, Tp, Value, Boundary;
	Boundary = Type.substr(Type.find(";") + 11);
	while (std::getline(file, line)) {
		if (line.substr(2) == Boundary) {
			while (std::getline(file, line) && !line.substr(0, line.length() - 1).empty()) {
				Key = line.substr(0, line.find(':'));
				Tp = line.substr(line.find(':') + 1);
				_body.insert(std::make_pair(Key, Tp));
			}
		}
		else {
			std::string Content = _body["Content-Disposition"];
			std::string _nameFile = Content.substr(Content.rfind(';') + 12, (Content.rfind('"') - (Content.rfind(';') + 12)));
			_contentFile = line.substr(0, line.length() - line.find('\r') + 3);
			std::cout << _contentFile << std::endl;
		}
	}
}

void Request::parsApplication(std::stringstream& file, std::string& line) {
	std::string Key, Tp, value;
	
	std::getline(file, line);
	if (line.find('&') != NOT_FOUND) {
		while (!line.empty()) {
			// std::cout << line << '\n';
			if (line.rfind('&') != NOT_FOUND) {
				value = line.substr(line.rfind('&') + 1);
				line.erase(line.rfind('&'), value.length() + 1);
			}
			else {
			 	value = line;
				line.erase(0, value.length());
			}
			Key = value.substr(0, value.find('='));
			Tp = value.substr(value.find('=') + 1);
			_body.insert(std::make_pair(Key, Tp)); 
		}
	}
	else {
		Key = line.substr(0, line.find('='));
		Tp = line.substr(line.find('=') + 1);
		_body.insert(std::make_pair(Key, Tp));
	}
}

void Request::parsPost(std::stringstream& file, std::string& line) {
	std::string body, value, Key, Tp;
	while (std::getline(file, line) && !line.substr(0, line.length() - 1).empty()) {
		size_t colon_pos = line.find(':');
		if (colon_pos != NOT_FOUND) {
			std::string Key = line.substr(0, colon_pos);
			std::string Tp = line.substr(colon_pos + 2);
			_headers.insert(std::make_pair(Key, Tp));
		}
	}
	if (_headers["Content-Type"] == "application/x-www-form-urlencoded") // name=aegfs&email=fasefe
		parsApplication(file, line);
	else if (_headers["Content-Type"].find("multipart/form-data") != NOT_FOUND)
		parsMultipart(file, line, _headers["Content-Type"]);
	// else if (_headers["Content-Type"] == "application/json") // 
	// 	parsJSon(file, line);
	// else if (_headers["Content-Type"] == "application/xml")
	// 	parsXml(file, line);
	// else if (_headers["Content-Type"] == "text/plain")
	// 	parsText(file, line);
	// else if (_headers["Content-Type"] == "application/octet-stream")
	// 	parsOctet(file, line);
	// else if (_headers["Content-Type"] == "application/ld+json")
	// 	parsLdJson(file, line);
	// else if (_headers["Content-Type"] == "text/csv")
	// 	parsTextCsv(file, line);
	// else if (_headers["Content-Type"] == "application/graphql")
	// 	parsGraph(file, line);
	// else if (_headers["Content-Type"] == "application/protobuf")
	// 	parsProtobuf(file, line);
	// else if (_headers["Content-Type"] == "text/event-stream")
	// 	parsEventStream(file, line);
	// else if (_headers["Content-Type"] == "application/zip")
	// 	parsZip(file, line);
}

void Request::parsDelete(std::stringstream& file, std::string& line) {
	(void)file;
	(void)line;
	std::cout << "parsDelete\n";
}

void Request::ParsRequest(std::string& to_pars) {
	std::stringstream ss(to_pars);
	std::string line;
	std::getline(ss, line);
	std::stringstream req_line(line);
	// std::cout << "______RICHIESTA NON PARSATA______\n" << to_pars << "\n_______________\n";
	req_line >> _method >> _url >> _httpVersion;
	if (_method == "GET")
		parsGet(ss, line);
	else if (_method == "POST")
		parsPost(ss, line);
	else if (_method == "DELETE")
		parsDelete(ss, line);
}

void Request::getRequest(int &client_socket, short& event, int MaxSize) {
	int bytes_recived = 0, total_bytes = 0;
	char *tmp_buffer = new char[MaxSize];
	std::string buffer;
	while ((bytes_recived = recv(client_socket, tmp_buffer, MaxSize, 0)) > 0) {
		total_bytes += bytes_recived;
		std::cout << "BYTE RECIVED: " << bytes_recived << '\n';
		std::cout << "REQUEST: " << tmp_buffer << '\n';
		buffer += tmp_buffer;
		delete [] tmp_buffer;
		tmp_buffer = new char[MaxSize];

	}
	if (total_bytes < 0) {
		throw exc("Error reading request\n");
	}
	ParsRequest(buffer);
	// printRequest();
	event = POLLOUT;
}


void Request::printRequest() {
	// std::cout << "Method: " + _method + '\n';
	// std::cout << "URL: " + _url + '\n';
	// std::cout << "HTTP Version: " + _httpVersion + '\n';
	// std::cout << "Headers: \n";
	// for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	// 	std::cout << it->first + ": " + it->second + '\n';
	// std::cout << "Body: \n";
	// for (std::map<std::string, std::string>::iterator bit = _body.begin(); bit != _body.end(); bit++)
	// 	std::cout << bit->first + ": " + bit->second + '\n';
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