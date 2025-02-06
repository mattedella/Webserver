
#include "../includes/webserv.hpp"
#include <iterator>
#include <map>
#include <string>
#include <vector>

ABlock::ABlock() {
	if (_special.empty()) {
		_special.push_back("http");
		_special.push_back("client_max_body_size");
		_special.push_back("error_page");
		_special.push_back("include");
		_special.push_back("server");
		_special.push_back("listen");
		_special.push_back("server_name");
		_special.push_back("root");
		_special.push_back("index");
		_special.push_back("autoindex");
		_special.push_back("location");
		_special.push_back("client_max_request");
		_special.push_back("cgi_index");
		_special.push_back("cgi_param");
		_special.push_back("cgi_pass");
		_special.push_back("try_files");
		_special.push_back("upload_pass");
		_special.push_back("upload_store");
		_special.push_back("dav_methods");
	}
};

int ABlock::ErrorPageSize() {
	return _error.size();
}

void ABlock::printMap()
{
	for (std::map<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it) {
        std::cout<<it->first<<" -> "<<it->second<<"\n";
    }
}

bool ABlock::someInfo(std::string &str)
{
	if (str.find('#') != NOT_FOUND && str.find('#') < str.find(';'))
		return false;
	for (int i = 0; str[i]; i++)
	{
		if (!std::isspace(str[i]) && str[i] != '{' && str[i] != '}')
			return true;
	}
	return false;
}

bool ABlock::checkKey(std::string& Key) {
	for (std::vector<std::string>::iterator it = _special.begin(); it != _special.end(); it++) {
		if (Key == *it)
			return true;
	}
	return false;
}

void ABlock::initMap(std::string& str) {
	if (str.empty())
		return ;

	if (!someInfo(str))
		return;
	std::string Key;
	std::string Tp;
	while(std::isspace(str[0]))
		str.erase(0, 1);
	if (str.find(' ') != NOT_FOUND)
		Key = str.substr(0, str.find(' '));
	else if (str.find('\t') != NOT_FOUND)
		Key = str.substr(0, str.find('\t'));
	else
	 	throw exc("Error: line not valid: " + str + '\n');
	if (Key.size() == 0)
		throw exc("Error: wrong key value " + str + "\n");
	if (checkKey(Key) == false)
		throw exc("Error: Key not valid: " + Key + '\n');
	if (str.find(' ') != NOT_FOUND)
		Tp = str.substr(str.find(' ') + 1, ((str.find(';')) - str.find(' ')) - 1);
	else if (str.find('\t') != NOT_FOUND)
		Tp = str.substr(str.find('\t') + 1, ((str.find(';')) - str.find('\t')) - 1);
	else
	 	throw exc("Error: line not valid: " + str + '\n');
	if (Tp.size() == 0)
		throw exc("Error: wrong value " + str + "\n");
	_data.insert(std::make_pair(Key, Tp));
}

size_t ABlock::getSize() const {
	return _data.size();
}

std::string ABlock::getRoot() {
	return _root;
}

std::string ABlock::getErrorPage(int error) {
	for (std::map<int, std::string>::iterator it = _error.begin(); it != _error.end(); it++)
		if (it->first == error)
			return it->second;
	
	return "";
}

bool ABlock::getMethods(std::string to_find) {
	for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++) {
		if (*it == to_find)
			return true;
	}
	return false;
}

int ABlock::getBodysize() {
	return _bodysize;
}

int ABlock::getTimeout() {
	return _timeout;
}

int ABlock::getMethodsSize() {
	return _methods.size();
}

ABlock::~ABlock() {}