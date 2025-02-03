
#include "../includes/webserv.hpp"
#include <bits/c++config.h>
#include <cerrno>
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

Request::Request() {
	_method = "";
	_url = "";
	_httpVersion = "";
}

std::string Request::generateBody() {
	std::string ret;
	ret = "{\r\n \"fileName\": \"" + getFileName() +
			"\",\r\n \"fileType\": \""+ getBody("Content-Type") +"\",\r\n"
			" \"operation\": \"upload\",\r\n"
			" \"status\": \"success\"\r\n}\r\n\r\n";
	return ret;
}

std::string Request::getFileName() {
	return _nameFile;
}

std::string Request::getBody(const std::string& Key) {
	return _body[Key];
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
	// TODO: iniziare;
	(void)file;
	(void)line;
	std::cout << "parsDelete\n";
}

void Request::parsApplication(std::stringstream& bodyData, std::string& line, std::string Path) {
	// TODO: salvare i dati da qualche parte(non so dove e come);
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
	_PostFile.open((Path + "/" + "data.txt").c_str(), std::ios::out | std::ios::app);
	for (std::map<std::string, std::string>::iterator it = _body.begin(); it != _body.end(); it++) {
		std::string to_write = it->first + "=" + it->second + '\n';
		_PostFile.write(to_write.c_str(), to_write.length());
	}
	_PostFile.write("------------\n", 13);
	_PostFile.close();
}

void Request::parsMultipart(std::stringstream& bodyData, std::string& Path, std::string Type) {
	std::string Value, Boundary, endBoundary;
	
	size_t boundaryPos = Type.find("boundary=");
	if (boundaryPos != NOT_FOUND) {
		Boundary = "--" + Type.substr(boundaryPos + 9);
		endBoundary = Boundary + "--";
	}
	else
		throw exc("Boundary not found in Content-Type");

	std::string body = bodyData.str();
	size_t startPos = body.find(Boundary);
	size_t lastPos = 0;

	while (startPos != std::string::npos && startPos > lastPos) {
		lastPos = startPos;

		if (body.substr(startPos, endBoundary.length()) == endBoundary) {
			break;
		}

		size_t headerStart = body.find("\r\n", startPos) + 2;
		size_t headerEnd = body.find("\r\n\r\n", headerStart);
		if (headerEnd == std::string::npos)
			break;
		
		size_t contentStart = headerEnd + 4;
		size_t contentEnd = body.find(Boundary, contentStart) - 2;
		if (contentEnd == std::string::npos || contentEnd <= contentStart) break;

		std::string headers = body.substr(headerStart, headerEnd - headerStart);
		size_t filenamePos = headers.find("filename=\"");
		size_t TypePos = headers.find("Content-Type:");
		if (filenamePos != std::string::npos) {
			std::string filename = headers.substr(
				filenamePos + 10,
				headers.find('"', filenamePos + 10) - (filenamePos + 10)
			);
			_nameFile = filename;
		if (TypePos != NOT_FOUND) {
			std::string contentType = headers.substr(TypePos + 14, headers.find("\r\n") - (TypePos + 14));
			_body.insert(std::make_pair("Content-Type", contentType));
		}
			// Write file content
			std::string content = body.substr(contentStart, contentEnd - contentStart);
			_PostFile.open((Path + "/" + _nameFile).c_str(), 
							std::ios::binary | std::ios::trunc);
			if (!_PostFile.is_open()) {
				throw exc("Error: cannot open the file\n");
			}
			_PostFile.write(content.c_str(), content.length());
		}
		
		startPos = body.find(Boundary, contentEnd);
	}
	_PostFile.close();
}

std::ofstream& Request::getPostFile() {
	return _PostFile;

}

void Request::closeFile() {
	_PostFile.close();
}

void Request::parsPost(std::stringstream& file, std::string& line, std::string Path) {
	std::string body, value, Key, Tp;
	while (std::getline(file, line) && !line.substr(0, line.length() - 1).empty()) {
	}
	if (_headers["Content-Type"].find("application/x-www-form-urlencoded") == 0)
		parsApplication(file, line, Path);
	else if (_headers["Content-Type"].find("multipart/form-data") == 0)
		parsMultipart(file, Path, _headers["Content-Type"]);
}

void Request::ParsRequest(std::stringstream& to_pars, conf* ConfBlock) {
	// TODO: Parsing per la CGI;
	std::string line = "";
	std::getline(to_pars, line);
	// std::cout << line << '\n';
	std::stringstream req_line(line);
	// std::cout << req_line.str() << std::endl;
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
	std::stringstream buffer;
	size_t total_received = 0, content_length = 0, header_end = std::string::npos;
	bool headers_complete = false;
	std::string temp_buffer;

	while (true) {
		char* chunk = new char[MaxSize];
		int bytes_received = recv(client_socket, chunk, MaxSize, 0);
		if (bytes_received <= 0 || bytes_received == MaxSize) {
			int err = errno;
			if (err == EAGAIN)
				std::cout << "EAGAIN No data available, try again later." << std::endl;
			else if (err == EWOULDBLOCK)
				std::cout << "EWOULDBLOCK No data available, try again later." << std::endl;
			else if (err == ECONNRESET)
				std::cout << "Connection reset by peer." << std::endl;
			delete[] chunk;
			break;
		}
		buffer.write(chunk, bytes_received);
		delete[] chunk;
		total_received += bytes_received;
		if (!headers_complete) {
			temp_buffer = buffer.str();
			header_end = temp_buffer.find("\r\n\r\n");
			if (header_end != std::string::npos) {
				headers_complete = true;
				std::string headers = temp_buffer.substr(0, header_end);
				size_t cl_pos = headers.find("Content-Length: ");
				if (cl_pos != std::string::npos) {
					size_t cl_end = headers.find("\r\n", cl_pos);
					std::stringstream ss(headers.substr(cl_pos + 16, cl_end - (cl_pos + 16)));
					ss >> content_length;
				}
			}
		}
		if (headers_complete && content_length > 0) {
			if (total_received >= content_length + header_end + 4) {
				break;
			}
		}
	}
	std::cout << buffer.str() << std::endl;
	ParsRequest(buffer, ConfBlock);
	event = POLLOUT;
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

void Request::setHeader(const std::string& Key, const std::string& Tp) {
	_headers.erase(Key);
	_headers.insert(std::make_pair(Key, Tp));
}

void Request::setContentType(const std::string& fullPath) {

	_headers.erase("Content-Type");
	if (fullPath.find(".html") != std::string::npos)
		_headers.insert(std::make_pair("Content-Type", "text/html"));
	else if (fullPath.find(".css") != std::string::npos)
		_headers.insert(std::make_pair("Content-Type", "text/css"));
	else if (fullPath.find(".js") != std::string::npos)
		_headers.insert(std::make_pair("Content-Type", "application/javascript"));
	else if (fullPath.find(".ico") != std::string::npos)
		_headers.insert(std::make_pair("Content-Type", "image/x-ico"));
	else if (fullPath.find(".png") != std::string::npos)
		_headers.insert(std::make_pair("Content-Type", "image/png"));
	else if (fullPath.find(".jpg") != std::string::npos || fullPath.find(".jpeg") != std::string::npos)
		_headers.insert(std::make_pair("Content-Type", "image/jpeg"));
	else
		_headers.insert(std::make_pair("Content-Type", "text/plain"));
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