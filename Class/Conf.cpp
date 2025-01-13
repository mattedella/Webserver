#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <iterator>
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

void conf::checkRequest(Request* req) { // magari aggiungere "int nbrServer" per sapere in che server siamo
	// cosi controlliamo solo i valori di quel determinato server invece che in tutti
	// capire se possiamo avere piu' HTTP (anche se penso di no)
	// e, se si, capire se conviene aggiungere i server all'interno di http
	// per semplicita' prendo solo il primo server
	std::string fullPath = _servers[0].getRoot();
	location loc;
	if (_servers[0].checkLocation(req->getURL()))
		loc = _servers[0].getLocation(req->getURL());
	else {
	 	StatusCode = 404;
		return ;
	}
	fullPath += loc.getRoot();
	fullPath += req->getURL();
	if (_http[0].getMethodsSize() > 0) {
		if (!_http[0].getMethods(req->getMethod()))
			StatusCode = 403;
		else
			StatusCode = 200;
	}
	if (_servers[0].getMethodsSize() > 0) {
		if (!_servers[0].getMethods(req->getMethod()) && StatusCode != 200)
			StatusCode = 403;
		else
			StatusCode = 200;
	}
	if (loc.getMethodsSize() > 0) {
		if (!loc.getMethods(req->getMethod()) && StatusCode != 200)
			StatusCode = 403;
		else
			StatusCode = 200;
	}
}

conf::~conf() {}