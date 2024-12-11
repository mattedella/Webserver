#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <string>
// #include <type_traits>

ABlock::ABlock() {};

void ABlock::initMap(std::string& str) {
	if (str.empty())
		return ;
	std::cout << "|" << str << "|\n";
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (!std::isspace(str[i]))
			break ;
		if (str[i + 1] == '\0')
			return ;
	}
	if (str.find('#') != std::string::npos)
		return ;
	std::string Key = str.substr(str.find('\t') + 1, str.find(' ') - 1);
	std::string Tp = str.substr(str.find(' ') + 1, ((str.find(' ')) - str.find(';') - 1) - 1);

	std::cout << "|" << Key << " = " << Tp << "|\n";

	_data[Key] = Tp;
}

ABlock::~ABlock() {}

http::http() : ABlock() {}

http::~http() {}