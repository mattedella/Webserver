
#include "../includes/webserv.hpp"
#include <fstream>
#include <sstream>

Response::Response() {}

	void Response::generateGetResponse(Request* req, conf ConfBlock) {

	std::ifstream file;
	std::stringstream buff;
	std::string request;
	switch (StatusCode) {
		case 200:
			file.open(ConfBlock.getFullPath().c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock.getFullPath() + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\nConnection: "
					+ req->getHeader("Connection") + "\r\n\r\n"
					+ request;
			break ;
		case 404:
			file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(404, 1, ConfBlock.getLocation(req->getURL(), 1))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock.getFullPath() + ConfBlock.getErrorPage(404, 1, ConfBlock.getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 404 Not Found\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 403:
			file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(403, 1, ConfBlock.getLocation(req->getURL(),1 ))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock.getFullPath() + ConfBlock.getErrorPage(403, 1, ConfBlock.getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 403 Forbidden\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 408:
			file.open((ConfBlock.getFullPath() + ConfBlock.getErrorPage(408, 1, ConfBlock.getLocation(req->getURL(), 1))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock.getFullPath() + ConfBlock.getErrorPage(408, 1, ConfBlock.getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 408 Request Timeout\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 501:
			std::string File = ConfBlock.getFullPath() + ConfBlock.getErrorPage(501, 1, ConfBlock.getLocation(req->getURL(), 1));
			file.open(File.c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock.getFullPath() + ConfBlock.getErrorPage(501, 1, ConfBlock.getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 501 Method not Allowed\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
	}
	std::cout << "--------RISPOSTA--------\n" << getResponse() << '\n';
	}

std::string Response::getResponse() {
	return _response;
}

Response::~Response() {}