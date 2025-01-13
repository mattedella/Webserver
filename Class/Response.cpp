
#include "../includes/webserv.hpp"
#include <fstream>
#include <sstream>

Response::Response() {}

void Response::generateResponse(Request* req, conf ConfBlock) {
	
	std::ifstream file;
	std::stringstream buff;
	std::string request;
	switch (StatusCode) {
		case 200:
			file.open(ConfBlock.getFullPath());
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 200 OK\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\n"
					+ request;
			break ;
		case 404:
			file.open(ConfBlock.getFullPath() + ConfBlock.getErrorPage(404, 1, ConfBlock.getLocation(req->getURL(), 1)));
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 404 Not Found\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\n"
					+ request;
			break ;
		case 403:
			file.open(ConfBlock.getFullPath() + ConfBlock.getErrorPage(403, 1, ConfBlock.getLocation(req->getURL(),1 )));
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 403 Forbidden\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\n"
					+ request;
			break ;
		case 408:
			file.open(ConfBlock.getFullPath() + ConfBlock.getErrorPage(408, 1, ConfBlock.getLocation(req->getURL(), 1)));
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 408 Request Timeout\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\n"
					+ request;
			break ;
		case 500:
			file.open(ConfBlock.getFullPath() + ConfBlock.getErrorPage(500, 1, ConfBlock.getLocation(req->getURL(), 1)));
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 500 Internal Server Error\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\n"
					+ request;
			break ;
	}
}

std::string Response::getResponse() {
	return _response;
}

Response::~Response() {}