
#include "../includes/webserv.hpp"
//#include <bits/posix_opt.h>
#include <dirent.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

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
	switch (StatusCode) {
		case 200:
		request = req->generateBody();
		_response = "HTTP/1.1 200 OK\r\n";
		break ;

	}
}
void Response::generateGetResponse(Request* req, conf* ConfBlock) {
    std::ifstream file;
    std::stringstream buff;
    std::string Path = ConfBlock->getFullPath();
    std::string request;
    std::string errorPath;

    // Usa la notazione originale per getErrorPage
    std::string error404 = ConfBlock->getErrorPage(404, 1, ConfBlock->getLocation(req->getURL(), 1));
    std::string error403 = ConfBlock->getErrorPage(403, 1, ConfBlock->getLocation(req->getURL(), 1));
    std::string error408 = ConfBlock->getErrorPage(408, 1, ConfBlock->getLocation(req->getURL(), 1));
    std::string error5xx = ConfBlock->getErrorPage(501, 1, ConfBlock->getLocation(req->getURL(), 1));

    struct dirent* readDir;
    DIR* dir = NULL;
    req->setContentType(Path);

    switch (StatusCode) {
        case 200: 
            if (Path.find('.') != NOT_FOUND) {
                file.open(Path.c_str());
                if (!file.is_open()) 
                    throw exc("Error: file \"" + Path + "\" not opened\n");
                
                buff << file.rdbuf();
                request = buff.str();
            } 
            else {
                dir = opendir(Path.c_str());
                if (dir == NULL) {
                    throw exc("ERROR: directory \"" + Path + "\" not opened\n");
                }
                
                req->setHeader("Content-Type", "text/html");
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
                        + req->getHeader("Connection") + "\r\n\r\n" 
                        + request;
            
            if (dir != NULL)
                closedir(dir);
            break;

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
            break;

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
            break;

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
            break;

        case 501:
            std::string File = ConfBlock->getFullPath() + error5xx;
            file.open(File.c_str());
            
            if (!file.is_open()) 
                throw exc("Error: file \"" + File + "\" not opened\n");
            
            buff << file.rdbuf();
            request = buff.str();
            
            _response = "HTTP/1.1 501 Method Not Allowed\r\nContent-Type: " 
                        + req->getHeader("Content-Type") 
                        + "\r\nConnection: close\r\n\r\n" 
                        + request;
            break;
    }
}

void Response::generatePostResponse(Request* req, conf* ConfBlock) {
    std::ofstream file;
    std::stringstream buff;
    std::string request;

    // Usa la notazione originale per getErrorPage
    std::string error404 = ConfBlock->getErrorPage(404, 1, ConfBlock->getLocation(req->getURL(), 1));
    std::string error403 = ConfBlock->getErrorPage(403, 1, ConfBlock->getLocation(req->getURL(), 1));
    std::string error408 = ConfBlock->getErrorPage(408, 1, ConfBlock->getLocation(req->getURL(), 1));
    std::string error5xx = ConfBlock->getErrorPage(501, 1, ConfBlock->getLocation(req->getURL(), 1));

    switch (StatusCode) {
        case 200: 
            request = req->generateBody();
            _response = "HTTP/1.1 201 Created\r\n" 
                        "Content-Type: " + req->getBody("Content-Type") + "\r\n" 
                        "Connection: " + req->getHeader("Connection") + "\r\n" 
                        "Content-Length: " + sizeToString(request.length()) + "\r\n" 
                        "\r\n" + request + "\r\n\r\n";
            break;

        case 404:
            file.open((ConfBlock->getFullPath() + error404).c_str());
            
            if (!file.is_open()) 
                throw exc("Error: file \"" + ConfBlock->getFullPath() + error404 + "\" not opened\n");
            
            buff << file.rdbuf();
            request = buff.str();
            
            _response = "HTTP/1.1 404 Not Found\r\nContent-Type: " 
                        + req->getHeader("Content-Type") 
                        + "\r\nConnection: close\r\n\r\n" 
                        + request;
            file.close();
            break;

        case 403:
            file.open((ConfBlock->getFullPath() + error403).c_str());
            
            if (!file.is_open()) 
                throw exc("Error: file \"" + ConfBlock->getFullPath() + error403 + "\" not opened\n");
            
            buff << file.rdbuf();
            request = buff.str();
            
            _response = "HTTP/1.1 403 Forbidden\r\nContent-Type: " 
                        + req->getHeader("Content-Type") 
                        + "\r\nConnection: close\r\n\r\n" 
                        + request;
            file.close();
            break;

        case 408:
            file.open((ConfBlock->getFullPath() + error408).c_str());
            
            if (!file.is_open()) 
                throw exc("Error: file \"" + ConfBlock->getFullPath() + error408 + "\" not opened\n");
            
            buff << file.rdbuf();
            request = buff.str();
            
            _response = "HTTP/1.1 408 Request Timeout\r\nContent-Type: " 
                        + req->getHeader("Content-Type") 
                        + "\r\nConnection: close\r\n\r\n" 
                        + request;
            file.close();
            break;

        case 501:
            std::string File = ConfBlock->getFullPath() + error5xx;
            file.open(File.c_str());
            
            if (!file.is_open()) 
                throw exc("Error: file \"" + File + "\" not opened\n");
            
            buff << file.rdbuf();
            request = buff.str();
            
            _response = "HTTP/1.1 501 Method Not Allowed\r\nContent-Type: " 
                        + req->getHeader("Content-Type") 
                        + "\r\nConnection: close\r\n\r\n" 
                        + request;
            file.close();
            break;
    }
    
    req->closeFile();
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
