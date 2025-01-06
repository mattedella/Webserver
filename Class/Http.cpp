
#include "../includes/webserv.hpp"
#include <map>
#include <vector>

http::http() : ABlock() {}

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

void http::printAll() {
	
	for (std::vector<int>::iterator it = _bodysize.begin(); it != _bodysize.end(); it++){
		std::cout << "body size: " << *it << '\n'; 
	}
	for (std::vector<int>::iterator it = _timeout.begin(); it != _timeout.end(); it++)
	{
		std::cout << "time out "<< *it << '\n';
	}
	for(std::vector<std::string>::iterator it = _include.begin(); it != _include.end(); it++)
		std::cout << "include: " << *it << '\n';
	std::cout<<"Errors :\n";
	for (std::map<int, std::string>::iterator it = _error.begin(); it != _error.end(); it++)
	{
		std::cout<< "num: " << it->first << " = " << it->second <<"\n";
	}
}

void http::addVal() {
	for (std::map<std::string, std::string>:: iterator it = _data.begin(); it != _data.end(); it++) {
		if (it->first.find("client_max_body_size") != NOT_FOUND) {
			int bodySize = std::atoi(it->second.c_str());
			if (it->second[it->second.length() - 1] != 'M' || bodySize <= 0)
				throw exc("Error: body size value not valid\n");
			_bodysize.push_back(bodySize);
			
		}
		if (it->first.find("client_body_timeout") != NOT_FOUND) {
			int timeout = std::atoi(it->second.c_str());
			if (it->second[it->second.length() - 1] != 's' || timeout <= 0)
				throw exc("Error: body timeout value not valid\n");
			_timeout.push_back(timeout);
		}
		if (it->first.find("include") != NOT_FOUND)
			_include.push_back(it->second);
	}
	_data.erase("client_max_body_size");
	_data.erase("client_body_timeout");
	_data.erase("include");
	if (_bodysize.size() <= 0)
		throw exc("Error: body size not found\n");
	if (_include.size() <= 0)
		throw exc("Error: include not found\n");
}

http::~http() {}