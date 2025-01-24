
#include "../includes/webserv.hpp"
#include <cstdio>
#include <fstream>
#include <ios>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>
#include <poll.h>
#include <sys/types.h>
#include <dirent.h>

Request::Request() {}

std::string Request::getFileName() {
	return _nameFile;
}

std::string Request::getBody(const std::string& Key) {
	return _body[Key];
}

void Request::setPostName(std::string Path) {
	_POSTFile.open((Path + "/" + _nameFile).c_str(), std::ios::binary);
	if (!_POSTFile.is_open()) {
		std::cout << "Error: cannot open the file\n";
	}
	std::cout << &_contentFile << " => " + _contentFile << std::endl;
	_POSTFile.write(_contentFile.c_str(), _contentFile.size());
	_POSTFile.close();
}

void Request::clear() {
	_method.clear();
	_headers.clear();
	_file.clear();
	_httpVersion.clear();
	_url.clear();
	_body.clear();
}

void Request::parsDelete(std::stringstream& file, std::string& line) {
	(void)file;
	(void)line;
	std::cout << "parsDelete\n";
}

void Request::parsApplication(std::stringstream& bodyData, std::string& line, std::string Path) {
	std::string Key, Tp, value;
	(void)Path;
	while (std::getline(bodyData, line) && !line.substr(0, line.length() - 1).empty())
		;
	std::getline(bodyData, line);
	if (line.find('&') != NOT_FOUND) {
		while (!line.empty()) {
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

void Request::parsMultipart(std::stringstream& bodyData, std::string& line, std::string Path, std::string Type) {
	std::string Value, Boundary, endBoundary;
	std::stringstream contentBody;

	size_t boundaryPos = Type.find("boundary=");
	if (boundaryPos != NOT_FOUND) {
		Boundary = "--" + Type.substr(boundaryPos + 9);
		endBoundary = Boundary + "--";
	}
	else
		throw exc("Boundary not found in Content-Type");

	std::string body = bodyData.str();
	size_t startPos = body.find(Boundary);
	while (startPos != std::string::npos) {
		startPos += Boundary.length();
		size_t endPos = body.find(Boundary, startPos);
		if (endPos == std::string::npos)
			break;

		std::string part = body.substr(startPos, endPos - startPos);
		contentBody << part;
		std::getline(contentBody, line);
		while (std::getline(contentBody, line) && !line.substr(0, line.length() - 1).empty()) {
			std::string Key = line.substr(0, line.find(':'));
			std::string tp = line.substr(line.find(':') + 2);
			_body.insert(std::make_pair(Key, tp));
		}

		size_t headerEnd = part.find("\r\n\r\n");
		if (headerEnd != std::string::npos) {
			std::string headers = part.substr(0, headerEnd);
			std::string _contentFile = part.substr(headerEnd + 4);
			size_t filenamePos = headers.find("filename=");
			if (filenamePos != std::string::npos) {
				std::string filename = headers.substr(
					filenamePos + 10, headers.find('"', filenamePos + 10) - (filenamePos + 10));
				_nameFile = filename;
				std::ofstream file((Path + "/" + _nameFile).c_str(), std::ios::binary);
				if (!file.is_open()) {
					throw exc("Error: cannot open the file");
				}
				file.write(_contentFile.c_str(), _contentFile.size());
				file.close();
			} 
			else
				std::cout << "No filename found in headers!" << std::endl;
		}
		startPos = endPos;
	}
}



void Request::parsPost(std::stringstream& file, std::string& line, std::string Path) {
	std::string body, value, Key, Tp;
	while (std::getline(file, line) && !line.substr(0, line.length() - 1).empty()) {
	}
	if (_headers["Content-Type"].find("application/x-www-form-urlencoded") == 0)
		parsApplication(file, line, Path);
	else if (_headers["Content-Type"].find("multipart/form-data") == 0)
		parsMultipart(file, line, Path, _headers["Content-Type"]);
	// else if (_headers["Content-Type"] == "application/json") // 
	// 	parsJSon(file, line, Path);
	// else if (_headers["Content-Type"] == "application/xml")
	// 	parsXml(file, line, Path);
	// else if (_headers["Content-Type"] == "text/plain")
	// 	parsText(file, line, Path);
	// else if (_headers["Content-Type"] == "application/octet-stream")
	// 	parsOctet(file, line, Path);
	// else if (_headers["Content-Type"] == "application/ld+json")
	// 	parsLdJson(file, line, Path);
	// else if (_headers["Content-Type"] == "text/csv")
	// 	parsTextCsv(file, line, Path);
	// else if (_headers["Content-Type"] == "application/graphql")
	// 	parsGraph(file, line, Path);
	// else if (_headers["Content-Type"] == "application/protobuf")
	// 	parsProtobuf(file, line, Path);
	// else if (_headers["Content-Type"] == "text/event-stream")
	// 	parsEventStream(file, line, Path);
	// else if (_headers["Content-Type"] == "application/zip")
	// 	parsZip(file, line, Path);
}

void Request::ParsRequest(std::stringstream& to_pars, conf* ConfBlock) {
	std::string line;
	std::getline(to_pars, line);
	std::stringstream req_line(line);
	req_line >> _method >> _url >> _httpVersion;
	while (std::getline(to_pars, line) && !line.substr(0, line.length() - 1).empty()) {
		size_t colon_pos = line.find(':');
		if (colon_pos != NOT_FOUND) {
			std::string Key = line.substr(0, colon_pos);
			std::string Tp = line.substr(colon_pos + 2, (line.length() - colon_pos) - 3);
			_headers.insert(std::make_pair(Key, Tp));
		}
	}
	ConfBlock->checkRequest(this);
	if (_method == "POST")
		parsPost(to_pars, line, ConfBlock->getFullPath());
	else if (_method == "DELETE")
		parsDelete(to_pars, line);
}

void Request::getRequest(int &client_socket, short& event, int MaxSize, conf* ConfBlock) {
	int bytes_received = 0;
	std::stringstream buffer;
	char* tmp_buffer = new char[MaxSize];

	bytes_received = recv(client_socket, tmp_buffer, MaxSize, 0);
	if (bytes_received < 0) {
		throw exc("Error reading request\n");
	}
	buffer.write(tmp_buffer, bytes_received);
	delete [] tmp_buffer;
	ParsRequest(buffer, ConfBlock);
	// printRequest();
	event = POLLOUT;
}

void Request::printRequest() {
	std::cout << "Method: " + _method + '\n';
	std::cout << "URL: " + _url + '\n';
	std::cout << "HTTP Version: " + _httpVersion + '\n';
	std::cout << "Headers: \n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first + ": " + it->second + '\n';
	std::cout << "Body: \n";
	for (std::map<std::string, std::string>::iterator bit = _body.begin(); bit != _body.end(); bit++)
		std::cout << bit->first + ": " + bit->second + '\n';
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