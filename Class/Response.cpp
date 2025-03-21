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
	std::ofstream file;
	std::stringstream buff;
	std::string request;
	(void)ConfBlock;

	request = req->generateDeleteBody();
	switch (StatusCode) {
		case 200:
			_response = "HTTP/1.1 200 OK\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += request;
			break;
		case 404:
			_response = "HTTP/1.1 404 Not Found\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += request;
			break;
		case 403:
			_response = "HTTP/1.1 403 Forbidden\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += request;
			break;
		case 500:
			_response = "HTTP/1.1 500 Internal Server Error\r\n";
			_response += "Content-Type: application/json\r\n";
			_response += "Content-Length: " + itos(request.length()) + "\r\n\r\n";
			_response += request;
			break;
	}
}
void Response::generatePostResponse(Request* req, conf* ConfBlock) {
	std::ofstream file;
	std::stringstream buff;
	std::string request;
	int nbrServer = ConfBlock->getNbrServer();
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
			file.open((ConfBlock->getFullPath() + ConfBlock->getErrorPage(404, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(404, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 404 Not Found\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			file.close();
			break ;
		case 403:
			file.open((ConfBlock->getFullPath() + ConfBlock->getErrorPage(403, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(403, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 403 Forbidden\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			file.close();
			break ;
		case 408:
			file.open((ConfBlock->getFullPath() + ConfBlock->getErrorPage(408, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer))).c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(408, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer)) + "\" not opened\n");
			buff << file.rdbuf();
			request = buff.str();
			_response = "HTTP/1.1 408 Request Timeout\r\nContent-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n\r\n"
					+ request;
			file.close();
			break ;
		case 501:
			std::string File = ConfBlock->getFullPath() + ConfBlock->getErrorPage(501, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer));
			file.open(File.c_str());
			if (!file.is_open())
				throw exc("Error: file \"" + ConfBlock->getFullPath() + ConfBlock->getErrorPage(501, nbrServer, ConfBlock->getLocation(req->getURL(), nbrServer)) + "\" not opened\n");
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
	int nbrServer = ConfBlock->getNbrServer();
	std::string error404 = ConfBlock->getErrorPage(404, nbrServer, ConfBlock->getLocation(req->getURL(),nbrServer));
	std::string error403 = ConfBlock->getErrorPage(403, nbrServer, ConfBlock->getLocation(req->getURL(),nbrServer));
	std::string error408 = ConfBlock->getErrorPage(408, nbrServer, ConfBlock->getLocation(req->getURL(),nbrServer));
	std::string error5xx = ConfBlock->getErrorPage(501, nbrServer, ConfBlock->getLocation(req->getURL(),nbrServer));
	req->setContentType(Path);
	switch (StatusCode) {
		case 200:
			request = ConfBlock->getResponseContent();
			if (req->getHeader("Connection").empty())
				req->setHeader("Connection", "close");
			_response = "HTTP/1.1 200 OK\r\nContent-Type: " + req->getHeader("Content-Type") + "\r\nConnection: "
						+ req->getHeader("Connection") + "\r\nContent-Length: " + itos(request.length()) +  "\r\n\r\n";
			_response += request;
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

void Response::generateCGIResponse(Request* req, conf* ConfBlock) {
    std::string url = req->getURL();
    std::string fullPath = ConfBlock->getFullPath();
    std::string queryString = "";
    size_t queryPos = url.find('?');
    if (queryPos != std::string::npos) {
        queryString = url.substr(queryPos + 1);
        url = url.substr(0, queryPos);
    }
    std::string scriptName = url.substr(url.find_last_of('/') + 1);
    std::string rootDir = fullPath;
    size_t filePos = fullPath.find("/File");
    if (filePos != std::string::npos) {
        rootDir = fullPath.substr(0, filePos + 5);
    }
    std::string scriptPath;
    if (url.find("/cgi-bin") != std::string::npos) {
        scriptPath = rootDir + "/cgi-bin/" + scriptName;
    } else if (url.find("/php") != std::string::npos) {
        scriptPath = rootDir + "/php/" + scriptName;
    } else {
        scriptPath = fullPath + url;
    }
    // scelta interprete
    std::string extension;
    size_t dot_pos = scriptName.find_last_of('.');
    if (dot_pos != std::string::npos)
        extension = scriptName.substr(dot_pos);
    std::string cgiPath;
    if (extension == ".php") {
        const char* phpPaths[] = {
            "/usr/bin/php-cgi", "/usr/bin/php",
            "/usr/local/bin/php-cgi", "/usr/local/bin/php"};
        for (int i = 0; i < 4 && cgiPath.empty(); i++) {
            std::ifstream check(phpPaths[i]);
            if (check) cgiPath = phpPaths[i];
        }
        if (cgiPath.empty()) cgiPath = "php";
    }
				else if (extension == ".py") {
        const char* pythonPaths[] = {
            "/usr/bin/python3", "/usr/bin/python",
            "/usr/local/bin/python3", "/usr/local/bin/python"
        };	
        for (int i = 0; i < 4 && cgiPath.empty(); i++) {
            std::ifstream check(pythonPaths[i]);
            if (check) cgiPath = pythonPaths[i];
        }
        if (cgiPath.empty()) cgiPath = "python";
    } else{
        _response = "HTTP/1.1 501 Not Implemented\r\n"
                   "Content-Type: text/html\r\n";
        std::string errorMsg = "<html><body><h1>501 Not Implemented</h1>"
                              "<p>CGI processing is only available for PHP and Python scripts.</p></body></html>";
        _response += "Content-Length: " + itos(errorMsg.length()) + "\r\n\r\n" + errorMsg;
        return;
    }
    // controllo esistenza script
    std::ifstream scriptFile(scriptPath.c_str());
    if (!scriptFile) {
        std::cerr << "CGI Script not found: " << scriptPath << std::endl;
        _response = "HTTP/1.1 404 Not Found\r\n"
                   "Content-Type: text/html\r\n";
        std::string errorMsg = "<html><body><h1>404 Not Found</h1>"
                              "<p>The requested CGI script was not found: " + scriptName + "</p>"
                              "<p>Path searched: " + scriptPath + "</p></body></html>";
        _response += "Content-Length: " + itos(errorMsg.length()) + "\r\n\r\n" + errorMsg;
        return;
    }
    scriptFile.close();
    try {
        CGIHandler cgi(cgiPath, scriptPath);
								//flag per evitare	doppia elaborazione della richiesta
        req->setHeader("X-CGI-Processed", "true");
        
        if (cgi.execute(req)) {
            std::string cgiOutput = cgi.getOutput();
            if (cgiOutput.substr(0, 4) == "HTTP") {
                _response = cgiOutput;
                return;
            }
            size_t header_end = cgiOutput.find("\r\n\r\n"); 
            if (header_end != std::string::npos) {
                std::string headers = cgiOutput.substr(0, header_end);
                std::string body = cgiOutput.substr(header_end + 4);
                _response = "HTTP/1.1 200 OK\r\n";
                bool contentTypeFound = false;
                std::istringstream header_stream(headers);
                std::string header_line;
                while (std::getline(header_stream, header_line) && !header_line.empty()) {
                    if (header_line.find("Content-Type:") != std::string::npos)
                        contentTypeFound = true;
                    _response += header_line + "\r\n";
                }
                if (!contentTypeFound)
                    _response += "Content-Type: text/html\r\n";
                _response += "Content-Length: " + itos(body.length()) + "\r\n\r\n" + body;
            } else {
                _response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: " + itos(cgiOutput.length()) + "\r\n\r\n" + cgiOutput;
            }
        } else {
            _response = "HTTP/1.1 500 Internal Server Error\r\n"
                       "Content-Type: text/html\r\n";
            std::string errorMsg = "<html><body><h1>500 Internal Server Error</h1>"
                                  "<p>Error executing CGI script.</p></body></html>";
            _response += "Content-Length: " + itos(errorMsg.length()) + "\r\n\r\n" + errorMsg;
        }
    } catch (std::exception &) {
        _response = "HTTP/1.1 500 Internal Server Error\r\n"
                   "Content-Type: text/html\r\n";
        std::string errorMsg = "<html><body><h1>500 Internal Server Error</h1>"
                              "<p>CGI exception occurred.</p></body></html>";
        _response += "Content-Length: " + itos(errorMsg.length()) + "\r\n\r\n" + errorMsg;
    }
}

std::string Response::getResponse() {
	return _response;
}
std::string Response::sizeToString(size_t size) {
    std::stringstream ss;
    ss << size;
    return ss.str();
}
Response::~Response() {}
