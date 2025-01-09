#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
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


server conf::getServer(std::string port)
{
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second.getListen(port) == port)
			return it->second;
	}
	throw exc("ci pensiamo dopo\n");
}

std::map<int, server> conf::getMapServer()
{
	return _servers;
}

conf::~conf() {}