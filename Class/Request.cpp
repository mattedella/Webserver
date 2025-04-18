#include "../includes/webserv.hpp"
//#include <bits/c++config.h>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <fcntl.h>
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
	_file = "";
	_nameFile = "";
	_headers_complete = false;
	_body_complete = false;
	_content_length = 0;
}

 std::string Request::generateDeleteBody() {
	std::string ret;
	switch (StatusCode) {
		case 200:
			ret = "{ \"message\": \"Resource deleted successfully\" }\r\n";
			break;
		case 404:
			ret = "{ \"error\": \"Resource not found\" }\r\n";
			break;
		case 403:
			ret = "{ \"error\": \"Access forbidden\" }\r\n";
			break;
		case 500:
			ret = "{ \"error\": \"Internal server error\" }\r\n";
			break;
	}
	return ret;
}

std::string Request::generateBody() {
    std::string ret;
	switch (StatusCode) {
		case 200:
			ret = "{ \"message\": \"Resource deleted successfully\" }\r\n\r\n";
			break;
		case 404:
			ret = "{ \"error\": \"Resource not found\" }\r\n\r\n";
			break;
		case 403:
			ret = "{ \"error\": \"Access forbidden\" }\r\n\r\n";
			break;
		case 500:
			ret = "{ \"error\": \"Internal server error\" }\r\n\r\n";
			break;
	}
    return ret;
}

std::string Request::getFileName() {
	return _nameFile;
}

std::string Request::getBody(const std::string& Key) {
	return _body[Key];
}

void Request::clear() {
	_url.clear();
	_file.clear();
	_httpVersion.clear();
	_method.clear();
	_headers.clear();
	_body.clear();
	_nameFile.clear();
	_headers_complete = false;
	_body_complete = false;
	_content_length = 0;
	StatusCode = 0;
}

void Request::parsApplication(std::stringstream& bodyData, std::string& line, std::string Path, size_t contentLength) {
	std::string Key, Tp, value;
	(void)Path;
	while (std::getline(bodyData, line) && !line.substr(0, line.length() - 1).empty())
		;
	std::getline(bodyData, line);
	if (line.length() < contentLength) {
		StatusCode = 400;
		return;
	}
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

void Request::parsMultipart(std::stringstream& bodyData, std::string& Path, std::string Type, size_t contentLength) {
	std::string Value, Boundary, endBoundary;
	// (void)contentLength;
	size_t boundaryPos = Type.find("boundary=");
	if (boundaryPos != NOT_FOUND) {
		Boundary = "--" + Type.substr(boundaryPos + 9);
		endBoundary = Boundary + "--\r\n";
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
		if (contentEnd == std::string::npos || contentEnd <= contentStart)
			break;
		if ((body.find(endBoundary) + endBoundary.length()) - body.find(Boundary) < contentLength) {
			StatusCode = 400;
			return ;
		}

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

void Request::parsPost(std::stringstream& file, std::string& line, std::string Path, size_t contentLength) {
	std::string body, value, Key, Tp;
	while (std::getline(file, line) && !line.substr(0, line.length() - 1).empty()) {
	}
	if (_headers["Content-Type"].find("application/x-www-form-urlencoded") == 0)
		parsApplication(file, line, Path, contentLength);
	else if (_headers["Content-Type"].find("multipart/form-data") == 0)
		parsMultipart(file, Path, _headers["Content-Type"], contentLength);
}

void Request::ParsRequest(std::stringstream& to_pars, conf* ConfBlock, size_t contentLength) {
	std::string line = "";
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
	if (!_headers.empty())
		ConfBlock->checkRequest(this, contentLength);
	if (_method == "POST" && StatusCode == 200)
		parsPost(to_pars, line, ConfBlock->getFullPath(), contentLength);
}

void Request::setMethod(std::string to_set) {
	_method = to_set;
}

void Request::getRequest(int client_socket, short& event, int MaxSize, conf* ConfBlock) {
	std::stringstream buffer;
	(void)MaxSize;
	int i = -1;
	size_t total_received = 0, content_length = 0, header_end = std::string::npos, chunkSize = 1024;
	bool headers_complete = false;
	std::string temp_buffer;

	while (total_received < content_length + header_end + 4) {
		i++;
		char* chunk = new char[chunkSize];
		int bytes_received = recv(client_socket, chunk, chunkSize, 0);
		if (bytes_received == 0 || i > 100) {
			event = 0;
			delete [] chunk;
			break ;
		}
		else if (bytes_received < 0) {
			delete[] chunk;
			continue;
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
	}
	if (total_received > 0) {
		ParsRequest(buffer, ConfBlock, content_length);
	}
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

const std::map<std::string, std::string>& Request::getHeaders() const {
    return _headers;
}

const std::map<std::string, std::string>& Request::getBodyMap() const {
    return _body;
}

std::string Request::getBodyContent() const {
    return _body_content;
}

void Request::setBodyContent(const std::string& content) {
    _body_content = content;
}

bool Request::isCGIRequest(const std::string& extension) const {
    if (_url.empty())
        return false;
        
    size_t dot_pos = _url.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string file_ext = _url.substr(dot_pos);
        return (file_ext == extension);
    }
    return false;
}

Request::~Request() {}