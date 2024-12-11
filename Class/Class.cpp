#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <string>
// #include <type_traits>

ABlock::ABlock() {};

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
	if (str.find('#') != std::string::npos && str.find('#') < str.find(';'))
		return ;
	while( std::isspace(str[0]))
		str.erase(0, 1);
	std::string Key = str.substr(0, str.find(' '));
	std::string Tp = str.substr(str.find(' ') + 1, ((str.find(';')) - str.find(' ')) - 1);
	std::cout << "|" << Key << " = " << Tp << "|\n";
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