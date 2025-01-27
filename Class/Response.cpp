
#include "../includes/webserv.hpp"
#include <bits/posix_opt.h>
#include <fstream>
#include <ostream>
#include <sstream>

Response::Response() {}

void Response::generatePostResponse(Request* req, conf* ConfBlock) {
	std::ofstream file;
	std::ofstream& postfile = req->getPostFile();
	std::stringstream buff;
	std::string request;
	switch (StatusCode) {
		case 200:
			buff << postfile.rdbuf();
			_response = "HTTP/1.1 201 Create\r\nContent-Type: " + req->getBody("Content-Type")
						+ "\r\nConnection: " + req->getHeader("Connection") + "\r\n\r\n"
						+ "{\r\n \"status\": \"success\",\r\n \"message\": Image uploaded successfully\",\r\n \"file_url\": \"http://" + req->getHeader("Host") + req->getURL() + "/" + req->getFileName() + "\"\r\n}\r\n\r\n"
						+ buff.str();
			break ;
		case 404:
			file.open((ConfBlock->getFullPath() + ConfBlock->getErrorPage(404, 1, ConfBlock->getLocation(req->getURL(), 1))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(404, 1, ConfBlock->getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 404 Not Found\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 403:
			file.open((ConfBlock->getFullPath() + ConfBlock->getErrorPage(403, 1, ConfBlock->getLocation(req->getURL(),1 ))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(403, 1, ConfBlock->getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 403 Forbidden\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 408:
			file.open((ConfBlock->getFullPath() + ConfBlock->getErrorPage(408, 1, ConfBlock->getLocation(req->getURL(), 1))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(408, 1, ConfBlock->getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 408 Request Timeout\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 501:
			std::string File = ConfBlock->getFullPath() + ConfBlock->getErrorPage(501, 1, ConfBlock->getLocation(req->getURL(), 1));
			file.open(File.c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(501, 1, ConfBlock->getLocation(req->getURL(), 1)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 501 Method not Allowed\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		}
		postfile.close();
		req->closeFile();
}

void Response::generateGetResponse(Request* req, conf* ConfBlock) {
	std::ifstream file;
	std::stringstream buff;
	std::string Path = ConfBlock->getFullPath();
	std::string request;
	std::string errorPath;
	std::string error404 = ConfBlock->getErrorPage(404, 1, ConfBlock->getLocation(req->getURL(), 1));
	std::string error403 = ConfBlock->getErrorPage(403, 1, ConfBlock->getLocation(req->getURL(), 1));
	std::string error408 = ConfBlock->getErrorPage(408, 1, ConfBlock->getLocation(req->getURL(), 1));
	std::string error5xx = ConfBlock->getErrorPage(501, 1, ConfBlock->getLocation(req->getURL(), 1));
	switch (StatusCode) {
		case 200:
			req->setContentType(Path);
			file.open(Path.c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + Path + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 200 OK\r\nContent-Type: " + req->getHeader("Content-Type") + "\r\nConnection: "
					+ req->getHeader("Connection") + "\r\n\r\n"
					+ request;
			break ;
		case 404:
			errorPath = Path.substr(0, Path.find("/File") + 5);
			req->setContentType(errorPath + error404);
			file.open((errorPath + error404).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + errorPath + error404 + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 404 Not Found\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 403:
			errorPath = Path.substr(0, Path.find("/File") + 5);
			req->setContentType(errorPath + error403);
			file.open((errorPath + error403).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + errorPath + error403 + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 403 Forbidden\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 408:
			errorPath = Path.substr(0, Path.find("/File") + 5);
			req->setContentType(errorPath + error408);
			file.open((errorPath + error408).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + errorPath + error408 + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 408 Request Timeout\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
		case 501:
			errorPath = Path.substr(0, Path.find("/File") + 5);
			std::cout << errorPath << std::endl;
			req->setContentType(errorPath + error5xx);
			std::string File = errorPath + error5xx;
			file.open(File.c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + errorPath + error5xx + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 501 Method not Allowed\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			break ;
	}
}

std::string Response::getResponse() {
	return _response;
}

Response::~Response() {}