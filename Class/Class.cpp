#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <map>
#include <string>
#include <utility>
// #include <type_traits>

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

ABlock::~ABlock() {}

http::http() : ABlock() {}

http::~http() {}

server::server() : ABlock() {}

void server::addLocation(std::string& Key, location loc) {
	_locations.insert(std::make_pair(Key, loc));
}

const std::map<std::string,std::string>::const_iterator server::findKey(const std::string& Key) const {
	const std::map<std::string,std::string>::const_iterator it = _data.find(Key);
	
	return it;
}

void server::printLocation() {
	for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::cout << it->first << '\n';
		it->second.printMap();
	}
}

size_t server::getSize() const {
	return _data.size();
}

server::~server() {}

location::location() : ABlock() {}

location::~location() {}

conf::conf() {}
conf::~conf() {}

void conf::addServer(const server &srv)
{
	if (srv.getSize() == 0)
		return ;
	std::map<std::string,std::string>::const_iterator it = srv.findKey("listen");
	_servers.insert(std::make_pair(it->second, srv));
}

void conf::printServer()
{
	for (std::map<std::string,server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		std::cout << it->first << '\n';
		it->second.printMap();
		std::cout << "---LOCATIONS---\n";
		it->second.printLocation();
		std::cout<<"\n";
    }
}