#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <iterator>
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
	if (_http[0].ErrorPageSize() > 0)
		errorPage = _http[0].getErrorPage(error);
	if (_servers[nbrServer].ErrorPageSize() > 0)
		errorPage = _servers[nbrServer].getErrorPage(error);
	if (location.ErrorPageSize() > 0)
		errorPage = location.getErrorPage(error);
	if (errorPage == "" && (error == 404 || error == 403 || error == 408))
		errorPage = "/400.html";
	if (errorPage == "" && error == 501)
		errorPage = "/500.html";
	return errorPage;
}

void conf::checkRequest(Request* req) { // magari aggiungere "int nbrServer" per sapere in che server siamo
	// cosi controlliamo solo i valori di quel determinato server invece che in tutti
	// per semplicita' prendo solo il primo server
	StatusCode = 200;
	char* buff = NULL;
	getcwd(buff, sizeof(buff));
	_fullPath = buff;
	location loc;
	if (_servers[0].checkLocation(req->getURL())) {
		loc = _servers[0].getLocation(req->getURL());
		_fullPath += loc.getRoot();
	}
	else {
	 	StatusCode = 404;
		return ;
	}
	_fullPath += req->getURL();

	if (_http[0].getMethodsSize() > 0)
		if (!_http[0].getMethods(req->getMethod()))
			StatusCode = 501;
	if (_servers[0].getMethodsSize() > 0)
		if (!_servers[0].getMethods(req->getMethod()))
			StatusCode = 501;
	if (loc.getMethodsSize() > 0)
		if (!loc.getMethods(req->getMethod()))
			StatusCode = 501;
	if (req->getURL() == "/") {
		if (!(_servers[0].getIndex() == "")) {
			if (!_servers[0].getListing())
				StatusCode = 404;
		}
		else
		 	_fullPath += _servers[0].getIndex();
	}
	else {
		if (loc.getIndex() == "") {
			if (!loc.getListing())
				StatusCode = 404;
		}
		else
			_fullPath += loc.getIndex();
	}

}

location conf::getLocation(std::string to_find, int nbrServer) {
	return _servers[nbrServer].getLocation(to_find);
}

std::string conf::getFullPath() {
	return _fullPath;
}

conf::~conf() {}