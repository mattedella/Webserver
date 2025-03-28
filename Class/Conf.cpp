#include "../includes/webserv.hpp"
#include <cctype>
#include <cstddef>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <unistd.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

conf::conf() {
	_listing = false;
}

void conf::addHost(server* server) {
		server->addNametoHost();
}

void conf::removeHosts(server* server) {
	server->removeNameToHost();
}

void conf::check()
{
	if (_http.size() == 0)
		throw exc ("Error: http not found!\n");
	if (_servers.size() == 0)
		throw exc ("Error: server not found!\n");
	for (std::vector<http>::iterator it =_http.begin(); it != _http.end(); ++it)
		it->checkVal();
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		it->second.checkValue();
	}
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		std::map<int, server>::iterator sec = it++;
		if (sec == _servers.end())
			break;
		it->second.checkPort(sec->second);
	}
}

void conf::addServer(int nbrServer, const server &srv)
{
	if (srv.getSize() == 0)
		return ;
	if (_servers.find(nbrServer) != _servers.end())
		_servers.erase(nbrServer);
	_servers.insert(std::make_pair(nbrServer, srv));
}

void conf::addHttp(const http& http) {
	if (_http.size() != 0)
		_http.pop_back();
	_http.push_back(http);
}

void conf::printServer()
{
	for (std::map<int,server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		std::cout << "Key == " << it->first << '\n';
		std::cout << "---LOCATIONS---\n";
		it->second.printLocation();
		std::cout<<"\n";
		it->second.printMap();
	}
	for (std::vector<http>::iterator it = _http.begin(); it != _http.end(); it++) {
		it->printMap();
	}
}

void conf::printHttp()
{
	std::vector<http>::iterator it = _http.begin();
	std::cout<<"----HTTP----\n";
	for (; it != _http.end(); it++)
		it->printAll();
	
}

void conf::addKey()
{
	for (std::map<int,server>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->second.addVal();
	for (std::vector<http>::iterator it = _http.begin(); it != _http.end(); it++)
		it->addVal();
}

server conf::getServer(int nbrServer) {
	server vd;
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
		if (it->first == nbrServer)
			return it->second;
	return vd;
}

int conf::getNbrServer() {
	return _nbrServer;
}

std::string conf::getErrorPage(int error, int nbrServer, location location) {
	char buff[4062];
	server& currentServer = _servers[nbrServer];
	getcwd(buff, sizeof(buff) - 1);
	std::string errorPage = buff;
	if (_http[0].ErrorPageSize() > 0 && _http[0].getErrorPage(error) != "")
		errorPage += _http[0].getErrorPage(error);
	if (currentServer.ErrorPageSize() > 0 && currentServer.getErrorPage(error) != "") {
		errorPage = buff;
		errorPage += currentServer.getRoot() + currentServer.getErrorPage(error);
	}
	if (location.ErrorPageSize() > 0 && location.getErrorPage(error) != "") {
		errorPage = buff;
		errorPage += currentServer.getRoot() + location.getRoot() + location.getErrorPage(error);
	}
	return errorPage;
}

void conf::checkRequest(Request* req, size_t contentLength) {
	_nbrServer = findServerByHostHeader(req);
	if (_nbrServer == -1)
		throw exc("No server found\n");
	StatusCode = 200;
	char buff[4062];
	struct dirent* readDir;
	std::string url = req->getURL();
	std::string subUrl;
	std::string file;
	std::stringstream responsebBuff;
	std::ifstream fileResponse;
	if (req->getURL().rfind(".") != NOT_FOUND || url.rfind('/') != url.length() - 1) {
		file = url.substr(url.rfind('/') + 1);
		url.erase(url.rfind('/') + 1, file.length() + 1);
	}
	if (req->getURL() != "/" && req->getURL()[req->getURL().length() - 1] == '/')
		subUrl = url.substr(0, url.rfind('/'));
	else
	 	subUrl = url.substr(0, url.find('/') + 1);
	server& currentServer = _servers[_nbrServer];
	getcwd(buff, sizeof(buff) - 1);
	_fullPath = buff;
	_fullPath += currentServer.getRoot();

	if (url != "/")
		subUrl = url.substr(0, url.rfind("/"));
	location loc;
	if (currentServer.checkLocation(subUrl))
		loc = currentServer.getLocation(subUrl);
	_fullPath += url;
	DIR* checkdir = opendir(_fullPath.c_str());
	if (checkdir == NULL)
		StatusCode = 404;
	closedir(checkdir);
	if (access(_fullPath.c_str(), R_OK | W_OK | X_OK) < 0)
		StatusCode = 403;
	if (_http[0].getMethodsSize() > 0)
		if (!_http[0].getMethods(req->getMethod()))
			StatusCode = 501;
	if (currentServer.getMethodsSize() > 0)
		if (!currentServer.getMethods(req->getMethod()))
			StatusCode = 501;
	if (loc.getMethodsSize() > 0)
		if (!loc.getMethods(req->getMethod()))
			StatusCode = 501;

	if (req->getMethod() == "GET") {
		if (file == "favicon.ico") {
			_fullPath = buff;
			_fullPath+= currentServer.getRoot() + "/favicon.ico";
			fileResponse.open(_fullPath.c_str());
			if (!fileResponse.is_open())
				StatusCode = 404;
			responsebBuff << fileResponse.rdbuf();
			_responseContent = responsebBuff.str();
			return ;
		}
		if (url == "/") {
			if (file.empty() && currentServer.getIndex() != "")
				_fullPath += currentServer.getIndex();
			else if (!file.empty())
				_fullPath += file;
			if (currentServer.getListing() == true)
				_listing = true;
			fileResponse.open(_fullPath.c_str());
			if (!fileResponse.is_open() && _listing == false)
				StatusCode = 404;
			else if (_listing == true) {
				if (_fullPath.find('.') != NOT_FOUND)
					_fullPath = _fullPath.substr(0 ,_fullPath.rfind('/'));
				DIR* dir = opendir(_fullPath.c_str());
				if (dir == NULL) {
					throw exc("ERROR: directory\"" + _fullPath + "\" not opened\n");
				}
				_fullPath += "/listing.html";
				req->setHeader("Content-type", "text/html");
				req->setHeader("Connection", "close");
				_responseContent = "<h1>OPS, the page you are loocking doesn't exist</h1>\r\n<p>try this:</p>\r\n";
				while ((readDir = readdir(dir)) != NULL) {
					_responseContent += "<a href=\"" + req->getHeader("Referer");
					_responseContent += readDir->d_name;
					_responseContent += "\">";
					_responseContent += readDir->d_name;
					_responseContent += "</a>\n<br>";
				}
				_responseContent += "\r\n\r\n";
				if (dir != NULL)
					closedir(dir);
				return ;
			}
			responsebBuff << fileResponse.rdbuf();
			_responseContent = responsebBuff.str();
		}
		else {
			if (file.empty() && loc.getIndex() != "") {
				_fullPath += loc.getIndex();
			}
			else if (!file.empty())
				_fullPath += file;
			else
			 	StatusCode = 404;
			if (currentServer.getListing() == true)
				_listing = true;
			if ((StatusCode != 404 && StatusCode != 403) && loc.getListing() == false)
				_listing = false;
			fileResponse.open(_fullPath.c_str());
			if (!fileResponse.is_open() && _listing == false)
				StatusCode = 404;
			else if (!fileResponse.is_open() && _listing == true) {
				if (_fullPath.find('.') != NOT_FOUND)
					_fullPath = _fullPath.substr(0 ,_fullPath.rfind('/') - 1);
				if (StatusCode == 404 || StatusCode == 403)
					_fullPath = buff + currentServer.getRoot();
				StatusCode = 200;
				DIR* dir = opendir(_fullPath.c_str());
				if (dir == NULL) {
					throw exc("ERROR: directory\"" + _fullPath + "\" not opened\n");
				}
				_fullPath += "listing.html";
				req->setHeader("Content-type", "text/html");
				req->setHeader("Connection", "close");
				_responseContent = "<h1>OPS, the page you are loocking doesn't exist</h1>\r\n<p>try this:</p>\r\n";
				while ((readDir = readdir(dir)) != NULL) {
					_responseContent += "<a href=\"" + req->getHeader("Referer");
					_responseContent += readDir->d_name;
					_responseContent += "\">";
					_responseContent += readDir->d_name;
					_responseContent += "</a>\n<br>";
				}
				_responseContent += "\r\n\r\n";
				if (dir != NULL)
					closedir(dir);
				return ;
			}
			responsebBuff << fileResponse.rdbuf();
			_responseContent = responsebBuff.str();
		}
	}

	else if (req->getMethod() == "POST") {
		if (_http[0].getBodysize() != 0) {
			if (_http[0].getBodysize() > contentLength)
				StatusCode = 200;
			else
				StatusCode = 413;
		}
		if (currentServer.getBodysize() != 0) {
			if (currentServer.getBodysize() > contentLength)
			StatusCode = 200;
			else
			StatusCode = 413;
		}
		if (loc.getBodysize() != 0) {
			if (loc.getBodysize() > contentLength)
				StatusCode = 200;
			else
				StatusCode = 413;
		}
		if (!file.empty() && StatusCode == 200)
			_fullPath += file;
	}

	else if (req->getMethod() == "DELETE") {
		if (_http[0].getMethodsSize() > 0) {
			if (!_http[0].getMethods(req->getMethod()))
				StatusCode = 500;
			else
				StatusCode = 200;
		}
		if (currentServer.getMethodsSize() > 0) {
			if (!currentServer.getMethods(req->getMethod()))
				StatusCode = 500;
			else
				StatusCode = 200;
		}
		if (loc.getMethodsSize() > 0) {
			if (!loc.getMethods(req->getMethod()))
				StatusCode = 500;
			else
				StatusCode = 200;
		}
		std::string fullPath = _fullPath+ req->getURL().substr(req->getURL().rfind('/') + 1, req->getURL().length() - req->getURL().rfind('/'));
		if (access(fullPath.c_str(), F_OK) != 0)
			StatusCode = 404;
		else if (access(fullPath.c_str(), W_OK) != 0)
			StatusCode = 403;
		else {
			if (remove(fullPath.c_str()) != 0)
				StatusCode = 500;
		}
	}
	else {
		req->setMethod("GET");
		StatusCode = 405;
	}
}

std::string conf::getResponseContent() {
	return _responseContent;
}

location conf::getLocation(std::string to_find, int nbrServer) {
	return _servers[nbrServer].getLocation(to_find);
}

std::string conf::getFullPath() {
	return _fullPath;
}

bool conf::getListing() {
	return _listing;
}

server conf::getServer(std::string port)
{
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second.getListen(port) == true)
			return it->second;
	}
	throw exc("ci pensiamo dopo\n");
}

std::map<int, server>& conf::getMapServer()
{
	return _servers;
}

int conf::findServerByHostHeader(Request* req) {
	std::string hostHeader = req->getHeader("Host");
	std::string portHeader, nameHeader;
	if (hostHeader.empty())
		return -1;
	size_t colonPos = hostHeader.find(':');
	if (colonPos != std::string::npos) {
		nameHeader = hostHeader.substr(0, colonPos);
		portHeader = hostHeader.substr(colonPos + 1,hostHeader.length() - colonPos);
	}
	for (std::map<int, server>::iterator it = _servers.begin(); 
		it != _servers.end(); ++it) {
		server& srv = it->second;
		bool serverPort = srv.getListen(portHeader);
		bool serverName = srv.getServerName(nameHeader);
		if (serverPort == true && serverName == true)
			return it->first;
	}
	return -1;
}

conf::~conf() {}
