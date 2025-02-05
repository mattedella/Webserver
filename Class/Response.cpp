
#include "../includes/webserv.hpp"
#include <bits/posix_opt.h>
#include <cstddef>
#include <dirent.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <variant>

Response::Response() {}

std::string itos(size_t nbr) {
	std::string ret;
	while (nbr > 0) {
		char num = (nbr % 10) + 48;
		nbr /= 10;
		ret.insert(ret.begin(), num);
	}
	return ret;
}

void Response::generateDeleteResponse(Request* req, conf* ConfBlock) {
	// TODO: capire come e' fatta la response e generarla;
	std::ofstream file;
	std::stringstream buff;
	std::string request;
	(void)ConfBlock;

	request = req->generateBody();
	switch (StatusCode) {
		case 200:
			std::cout << "req: " << request << "\n";
			_response = "HTTP/1.1 200 OK\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += "{ \"message\": \"Resource deleted successfully\" }\r\n";
			break;
		case 404:
			_response = "HTTP/1.1 404 Not Found\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += "{ \"error\": \"Resource not found\" }\r\n";
			break;
		case 403:
			_response = "HTTP/1.1 403 Forbidden\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += "{ \"error\": \"Access forbidden\" }\r\n";
			break;
		case 500:
			_response = "HTTP/1.1 500 Internal Server Error\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += "{ \"error\": \"Internal server error\" }\r\n";
			break;
	}
}
void Response::generatePostResponse(Request* req, conf* ConfBlock) {
	std::ofstream file;
	std::stringstream buff;
	std::string request;
	switch (StatusCode) {
		case 200:
			if (req->getHeader("Connection").empty())
				req->setHeader("Connection", "close"); 
			request = req->generateBody();
			_response = "HTTP/1.1 201 Created\r\n"
			"Content-Type: " + req->getBody("Content-Type") + "; charset=utf-8\r\n"
			"Connection: close" + req->getHeader("Connection") + "\r\n"
			"Content-Length: " + itos(request.length()) + "\r\n"
			"\r\n" + request + "\r\n\r\n";
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
			file.close();
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
			file.close();
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
			file.close();
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
			file.close();
			break ;
		}
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
	struct dirent* readDir;
	DIR* dir = NULL;
	req->setContentType(Path);
	switch (StatusCode) {
		case 200:
			if (req->getHeader("Connection").empty())
				req->setHeader("Connection", "close");
			if (Path.find('.') != NOT_FOUND) {
				file.open(Path.c_str());
				if (!file.is_open())
					throw exc("Error: file \"" + Path + "\" not opened\n");
				buff << file.rdbuf();
				request = buff.str();
				req->setContentType(Path);
			}
			else {
				dir = opendir(Path.c_str());
				if (dir == NULL) {
					throw exc("ERROR: directory\"" + Path + "\" not opened\n");
				}
				req->setHeader("Content-type", "text/html");
				req->setHeader("Connection", "close");
				request = "<h1>OPS, the page you are loocking doesn't exist</h1>\r\n<p>try this:</p>\r\n";
				while ((readDir = readdir(dir)) != NULL) {
					request += "<a href=\"" + req->getHeader("Referer");
					request += readDir->d_name;
					request += "\">";
					request += readDir->d_name;
					request += "</a>\n<br>";
				}
				request += "\r\n\r\n";
			}
			_response = "HTTP/1.1 200 OK\r\nContent-Type: " + req->getHeader("Content-Type") + "\r\nConnection: "
					+ req->getHeader("Connection") + "\r\nContent-Length: " + itos(request.length()) +  "\r\n\r\n" 
					+ request;
			
			if (dir != NULL)
				closedir(dir);
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
