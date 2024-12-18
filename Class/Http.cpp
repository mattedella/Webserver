
#include "../includes/webserv.hpp"

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

http::~http() {}