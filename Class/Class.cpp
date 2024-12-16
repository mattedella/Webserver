#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>


size_t server::checkLocation() const
{
	return _locations.size();
}

void conf::check()
{
	if (_http.size() == 0)
		throw exc ("Error: http not found!\n");
	if (_servers.size() == 0)
		throw exc ("Error: server not found!\n");
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if (it->second.checkLocation() == 0)
			throw exc("Error: location not found!\n");
	}
}

ABlock::ABlock() {};

void ABlock::printMap()
{
	for (std::map<std::string, std::string>::const_iterator it = _data.begin(); it != _data.end(); ++it) {
        std::cout<<it->first<<" -> "<<it->second<<"\n";
    }
}

bool ABlock::someInfo(std::string &str)
{
	if (str.find('#') != std::string::npos && str.find('#') < str.find(';'))
		return false;
	for (int i = 0; str[i]; i++)
	{
		if (!std::isspace(str[i]) && str[i] != '{' && str[i] != '}')
			return true;
	}
	return false;
}

void ABlock::initMap(std::string& str) {
	if (str.empty())
		return ;
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (!std::isspace(str[i]))
			break ;
		if (str[i + 1] == '\0')
			return ;
	}
	if (!someInfo(str))
		return;
	while( std::isspace(str[0]))
		str.erase(0, 1);
	std::string Key = str.substr(0, str.find(' '));
	std::string Tp = str.substr(str.find(' ') + 1, ((str.find(';')) - str.find(' ')) - 1);
	_data.insert(std::make_pair(Key, Tp));
}

size_t ABlock::getSize() const {
	return _data.size();
}

ABlock::~ABlock() {}

http::http() : ABlock() {}

void http::initVector() {
	for (std::multimap<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it) {
		if (it->first == "error_page") {
			std::string Key = it->second.substr(0, it->second.find(' '));
			int nbrKey = std::atoi(Key.c_str());
			std::string Tp = it->second.substr(it->second.find(' ') + 1);
			_error.insert(std::make_pair(nbrKey, Tp));
		}
	}
	_data.erase("error_page");
}

http::~http() {}

server::server() : ABlock() {}

void server::addLocation(const std::string& Key, location loc) {
	_locations.insert(std::make_pair(Key, loc));
}

const std::map<std::string,std::string>::const_iterator server::findKey(const std::string& Key) const {
	const std::map<std::string,std::string>::const_iterator it = _data.find(Key);
	
	return it;
}

void server::initVector() {
	if (_data.find("listen") == _data.end() || _data.find("server_name") == _data.end())
		throw exc("Error: no key listen or server found\n");
	
	for (std::multimap<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it) {
		if (it->first == "listen")
			_listens.push_back(it->second);
		else if (it->first == "server_name")
			_server_names.push_back(it->second);
	}
	_data.erase("listen");
	_data.erase("server_name");
}

void server::printLocation() {
	std::cout << _locations.size() << '\n';
	for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::cout << "Key -> " << it->first << '\n';
		it->second.printMap();
	}
	std::cout << "listen :\n";
	for (size_t i = 0; i < _listens.size(); i++)
		std::cout << _listens[i] << '\n';
	std::cout << "server name:\n";
	for (size_t i = 0; i < _server_names.size(); i++)
		std::cout << _server_names[i] << '\n';
	std::cout<<"Listing "<<_listing<<"\n";

}


server::~server() {}

location::location() : ABlock() {}

location::~location() {}

conf::conf() {}
conf::~conf() {}

void conf::addServer(int nbrServer, const server &srv)
{
	if (srv.getSize() == 0)
		return ;
	if (_servers.find(nbrServer) != _servers.end())
		_servers.erase(nbrServer);
	_servers.insert(std::make_pair(nbrServer, srv));
}

void conf::addHttp(const http& http) {
	if (http.getSize() == 0)
		return ;
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