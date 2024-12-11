#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <string>
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
	_data[Key] = Tp;
}

ABlock::~ABlock() {}

http::http() : ABlock() {}

http::~http() {}

server::server() : ABlock() {}

void server::addLocation(location loc) {
	_locations.push_back(loc);
}

server::~server() {}

location::location() : ABlock() {}

location::~location() {}