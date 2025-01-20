#include "../includes/webserv.hpp"
#include <cctype>
#include <unistd.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

conf::conf() {}

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
		it->second.printMap();
		std::cout << "---LOCATIONS---\n";
		it->second.printLocation();
		std::cout<<"\n";
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

std::string conf::getErrorPage(int error, int nbrServer, location location) { // fatto un po' a cazzo
	std::string errorPage = "";
	(void)nbrServer;
	if (_http[0].ErrorPageSize() > 0)
		errorPage = _http[0].getErrorPage(error);
	if (_servers[1].ErrorPageSize() > 0)
		errorPage = _servers[1].getErrorPage(error);
	if (location.ErrorPageSize() > 0)
		errorPage = location.getErrorPage(error);
	if (errorPage == "" && (error == 404 || error == 403 || error == 408))
		errorPage = "/40x.html";
	if (errorPage == "" && error == 501)
		errorPage = "/50x.html";
	return errorPage;
}

void conf::checkPostRequest(Request * req) {
	StatusCode = 200;
	char buff[_servers[1].getBodysize()*1000000];
	std::string url = req->getURL();
	std::string file;
	if (req->getURL().rfind(".") != NOT_FOUND) {
		file = url.substr(url.rfind('/') + 1);
		url.erase(url.rfind('/') + 1, (url.length() - url.rfind('/')));
	}
	if (req->getURL() != "/" && req->getURL()[req->getURL().length() - 1] == '/')
		url = req->getURL().substr(0, req->getURL().rfind('/'));
	std::string subUrl = url;
	getcwd(buff, sizeof(buff) - 1);
	_fullPath = buff;
	_fullPath += _servers[1].getRoot();
	if (url != "/")
		subUrl = url.substr(url.rfind("/"));
	location loc;
	if (_servers[1].checkLocation(subUrl)) {
		loc = _servers[1].getLocation(subUrl);
	}
	else {
	 	StatusCode = 404;
	}
	if (access(_fullPath.c_str(), R_OK | W_OK | X_OK) < 0)
		throw exc("ERROR: Cannot open the directory\n");
}

void conf::checkGetRequest(Request* req) { // magari aggiungere "int nbrServer" per sapere in che server siamo
	// cosi controlliamo solo i valori di quel determinato server invece che in tutti
	// per semplicita' prendo solo il primo server
	StatusCode = 200;
	char buff[4062];
	std::string url = req->getURL();
	std::string file;
	if (req->getURL().rfind(".") != NOT_FOUND) {
		file = url.substr(url.rfind('/') + 1);
		url.erase(url.rfind('/') + 1, (url.length() - url.rfind('/')));
	}
	if (req->getURL() != "/" && req->getURL()[req->getURL().length() - 1] == '/')
		url = req->getURL().substr(0, req->getURL().rfind('/'));
	std::string subUrl = url;
	getcwd(buff, sizeof(buff) - 1);
	_fullPath = buff;
	_fullPath += _servers[1].getRoot();
	if (url != "/")
		subUrl = url.substr(url.rfind("/"));
	location loc;
	if (_servers[1].checkLocation(subUrl)) {
		loc = _servers[1].getLocation(subUrl);
	}
	else {
	 	StatusCode = 404;
	}
	if (StatusCode == 200 && req->getURL().rfind('.') == NOT_FOUND)
		_fullPath += req->getURL();
	else
	 	_fullPath += req->getURL().substr(0, req->getURL().rfind("/") + 1);
	if (access(_fullPath.c_str(), R_OK | W_OK | X_OK) < 0)
		throw exc("ERROR: Cannot open the directory\n");
	if (_http[0].getMethodsSize() > 0)
		if (!_http[0].getMethods(req->getMethod())) {
			StatusCode = 501;
		}
	if (_servers[1].getMethodsSize() > 0)
		if (!_servers[1].getMethods(req->getMethod())) {
			StatusCode = 501;
		}
	if (loc.getMethodsSize() > 0)
		if (!loc.getMethods(req->getMethod())) {
			StatusCode = 501;
		}
	if (url == "/" && StatusCode == 200) {
		if (_servers[1].getIndex() == "") {
			if (!_servers[1].getListing())
				StatusCode = 404;
		}
		if (file.empty() && _servers[1].getIndex() != "")
			_fullPath += _servers[1].getIndex();
		else if (!file.empty() && file == _servers[1].getIndex())
		 	_fullPath += _servers[1].getIndex();
		else
		 	StatusCode = 404;
	}
	else if (StatusCode == 200) {
		if (loc.getIndex() == "") {
			if (!loc.getListing())
				StatusCode = 404;
		}
		else if (!file.empty() && file == loc.getIndex())
			_fullPath += loc.getIndex();
		else
		 	StatusCode = 404;
	}
}

location conf::getLocation(std::string to_find, int nbrServer) {
	return _servers[nbrServer].getLocation(to_find);
}

std::string conf::getFullPath() {
	return _fullPath;
}

server conf::getServer(std::string port)
{
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second.getListen(port) == port)
			return it->second;
	}
	throw exc("ci pensiamo dopo\n");
}

std::map<int, server>& conf::getMapServer()
{
	return _servers;
}

conf::~conf() {}