
#include "../includes/webserv.hpp"
#include <map>
#include <memory>
#include <vector>

http::http() : ABlock() {
	_bodysize = 30;
	_timeout = 60;
}

void http::initVector() {
	for (std::multimap<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it) {
		if (it->first == "error_page") {
			std::string Key = it->second.substr(0, it->second.find(' '));
			int nbrKey = std::atoi(Key.c_str());
			std::string Tp = it->second.substr(it->second.find(' ') + 1);
			_error.insert(std::make_pair(nbrKey, Tp));
		}
		else if (it->first.find("error_page") != NOT_FOUND && it->first != "error_page")
			throw exc("Error: line not valid : " + it->first + " " + it->second + '\n');
	}
	_data.erase("error_page");
}

void http::checkVal()
{
	for (std::map<int, std::string>::iterator it = _error.begin(); it != _error.end(); it++) {
		if (it->first != 404 && it->first != 403 && it->first != 405 && it->first != 413 && it->first != 500 && it->first != 501 && it->first != 408)
		 	throw exc("invalid error: " + it->second + "\n");
		if (it->second.find('.') == NOT_FOUND)
			throw exc("Error: invalid error page: " + it->second + "\n");
	}
	for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++)
		if (*it != "GET" &&	*it != "POST" && *it != "DELETE")
			throw exc("Error: method \"" + *it + "\" not valid\n");
}

void http::printAll() {
	
	std::cout << "body size: " << _bodysize << '\n'; 
		std::cout << "time out "<< _timeout << '\n';
	for(std::vector<std::string>::iterator it = _include.begin(); it != _include.end(); it++)
		std::cout << "include: " << *it << '\n';
	std::cout<<"Errors :\n";
	for (std::map<int, std::string>::iterator it = _error.begin(); it != _error.end(); it++)
	{
		std::cout<< "num: " << it->first << " = " << it->second <<"\n";
	}
	std::cout << "methods: ";
	for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++) {
		std::cout << *it << " ";
	}
	std::cout << '\n';
}

void http::addVal() {
	for (std::map<std::string, std::string>:: iterator it = _data.begin(); it != _data.end(); it++) {
		if (it->first.find("client_max_body_size") != NOT_FOUND) {
			int bodySize = std::atoi(it->second.c_str());
			if (it->second[it->second.length() - 1] != 'M' || bodySize <= 0)
				throw exc("Error: body size value not valid\n");
			_bodysize = bodySize;

		}
		if (it->first.find("include") != NOT_FOUND)
			_include.push_back(it->second);
		if (it->first.find("dav_methods") != NOT_FOUND) {
			std::string methods = it->second;
			while (!methods.empty()) {
				int i = 0;
				while (!std::isspace(methods[i]) || !methods[i])
					i++;
				std::string to_push = methods.substr(0, i);
				methods.erase(0, to_push.length() + 1);
				_methods.push_back(to_push);
			}
		}
	}
	_data.erase("client_max_body_size");
	_data.erase("include");
	if (_bodysize <= 0)
		throw exc("Error: body size not found\n");
}

std::string http::getInclude(std::string to_find) {
	for (std::vector<std::string>::iterator it = _include.begin(); it != _include.end(); it++)
		if (*it == to_find)
			return *it;
	return NULL;
}

http::~http() {}