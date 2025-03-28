
#include "../includes/webserv.hpp"
#include <cctype>
#include <cstddef>
#include <exception>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

location::location() : ABlock() {
	_listing = false;
	_index = "";
}

bool location::getListing() {
	return _listing;
}

std::string location::getIndex() {
	return _index;
}

void location::addVal() {
	for (std::map<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); it++) {
		if (it->first == "root") {
			if (it->second.find('/') == NOT_FOUND)
				throw exc("Error: root " + it->second + " not valid\n");
			_root = it->second;
		}
		else if (it->first == "index") {
			if (it->second.find('.') == NOT_FOUND)
				throw exc("Error: index " + it->second + " not valid\n");
			_index = it->second;
		}
		else if (it->first == "client_max_body_size") {
			int size = std::atoi(it->second.c_str());
			if (it->second[it->second.length() - 1] != 'M' || size <= 0)
				throw exc("Error: body size value not valid\n");
			_bodysize = size;
		}
		else if (it->first == "dav_methods") {
			std::string methods = it->second;
			while (!methods.empty()) {
				int i = 0;
				while (methods[i] && !std::isspace(methods[i]))
					i++;
				std::string to_push = methods.substr(0, i);
				if (to_push != "GET" && to_push != "POST" && to_push != "DELETE")
					throw exc("Error: method \"" + to_push + "\" is not valid\n");
				methods.erase(0, to_push.length() + 1);
				_methods.push_back(to_push);
			}
		}
		else if (it->first == "error_page") {
			std::string Key = it->second.substr(0, it->second.find(' '));
			int nbrKey = std::atoi(Key.c_str());
			std::string Tp = it->second.substr(it->second.find(' ') + 1);
			_error.insert(std::make_pair(nbrKey, Tp));
		}
		else if (it->first == "autoindex") {
			if (it->second == "on")
				_listing = true;
		}
	}
	_data.erase("root");
	_data.erase("error_page");
	_data.erase("index");
	_data.erase("client_maxz_body_size");
	_data.erase("dav_methods");
}

void location::printVal() {
	std::cout << "-----LOCATION------\n";
	std::cout << "root: " << _root << '\n';
	std::cout << "methods: ";
	for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++)
		std::cout << *it << " ";
	std::cout << '\n';
	std::cout << "index: " << _index << '\n';
	std::cout << "bodySize: " << _bodysize << '\n';
}

location::~location() {}

server::server() : ABlock(), _listing(false) {
	_index = "";
	_poll_fds = NULL;
}

size_t server::getLocationSize() const {
	return _locations.size();
}

void server::addLocation(const std::string& Key, location loc) {
	if (_locations.find(Key) != _locations.end())
		throw exc("Error: location already exist: " + Key + '\n');
	_locations.insert(std::make_pair(Key, loc));
}

void server::initVector() {
	if (_data.find("listen") == _data.end())
		_data.insert(std::make_pair("listen", "8080"));
	_data.insert(std::make_pair("server_name", "localhost"));

	for (std::multimap<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); ++it) {
		if (it->first == "listen") {
			for (std::vector<std::string>::iterator vec = _listens.begin(); vec != _listens.end(); ++vec) {
				if (it->second == *vec)
					throw exc ("Error: listen already exist: " + it->second + '\n');
			}
			_listens.push_back(it->second);
		}
		else if (it->first == "server_name") {
			for (std::vector<std::string>::iterator vec = _server_names.begin(); vec != _server_names.end(); ++vec) {
				if (it->second == *vec)
					throw exc ("Error: server name already exist: " + it->second + '\n');
			}
			std::string nameS = it->second;
			while (!nameS.empty()) {
				int i = 0;
				while (nameS[i] && !std::isspace(nameS[i]))
					i++;
				std::string to_push = nameS.substr(0, i);
				_server_names.push_back(to_push);
				nameS.erase(0, to_push.length() + 1);
			}
		}
		else if (it->first == "autoindex") {
			if (it->second == "on")
				_listing = true;
			else if (it->second != "off")
				throw exc("Invalid listing\n");
		}
		else if (it->second == "error_page") {
			std::string Key = it->second.substr(0, it->second.find(' '));
			int nbrKey = std::atoi(Key.c_str());
			std::string Tp = it->second.substr(it->second.find(' ') + 1);
			_error.insert(std::make_pair(nbrKey, Tp));
		}
		else if (it->first == "dav_methods") {
			std::string methods = it->second;
			int i = 0;
			while (!methods.empty()) {
				i = 0;
				while (methods[i] && !std::isspace(methods[i]))
					i++;
				std::string to_push = methods.substr(0, i);
				methods.erase(0, to_push.length() + 1);
				_methods.push_back(to_push);
			}
		}
		else if (it->first == "error_page") {
			std::string Key = it->second.substr(0, it->second.find(' '));
			int nbrKey = std::atoi(Key.c_str());
			std::string Tp = it->second.substr(it->second.find(' ') + 1);
			_error.insert(std::make_pair(nbrKey, Tp));
		}
		else if (it->first.find("error_page") != NOT_FOUND && it->first != "error_page")
			throw exc("Error: line not valid : " + it->first + " " + it->second + '\n');
	}
	_data.erase("error_page");
	_data.erase("listen");
	_data.erase("server_name");
	_data.erase("autoindex");
	_data.erase("dav_methods");
}

void server::printLocation() {
	std::cout << _locations.size() << '\n';
	for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::cout << "Key -> " << it->first << '\n';
		it->second.printVal();
	}
	std::cout << "listen:\n";
	for (size_t i = 0; i < _listens.size(); i++)
		std::cout << _listens[i] << '\n';
	std::cout << "server name:\n";
	for (size_t i = 0; i < _server_names.size(); i++)
		std::cout << _server_names[i] << '\n';
	std::cout << "listing: " << _listing << '\n';
}

void server::printAll()
{
	std::cout << "PRINT ALL\n";
	std::cout << "Server\n";
	std::cout << "time out "<<_timeout << '\n';
	std::cout << "root "<<_root << '\n';
	std::cout << "max body size "<<_bodysize << '\n';
	std::cout << "listing "<<_listing << '\n';
	std::cout << "index "<<_index << '\n';
	std::cout << "listen:\n";
	for (size_t i = 0; i < _listens.size(); i++)
		std::cout << _listens[i] << '\n';
	std::cout << "server name:\n";
	for (size_t i = 0; i < _server_names.size(); i++)
		std::cout << _server_names[i] << '\n';
	std::cout << "methods: ";
	for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++) {
		std::cout << *it << " ";
	}
	std::cout << '\n';
	std::cout << "MAP\n";
	printMap();
	std::cout << "LOCATIONS\n";
	printLocation();
}

void server::addVal()
{
	std::multimap<std::string, std::string>::iterator it;

	while ((it = _data.find("listen")) != _data.end()) {
		if (it->second.find(":") != NOT_FOUND)
			it->second = it->second.substr(it->second.find(":") + 1);
		std::cout << it->second << '\n';
        _listens.push_back(it->second);
        _data.erase(it);
    }
	while ((it = _data.find("server_name")) != _data.end()) {
		_server_names.push_back(it->second);
		_data.erase(it);
	}
	it = _data.find("client_max_body_size");
	if (it != _data.end()) {
		int size = std::atoi(it->second.c_str());
		if (it->second[it->second.length() - 1] != 'M' || size <= 0)
				throw exc("Error: body size value not valid\n");
		_bodysize = size;
		_data.erase(it);
	}
	it = _data.find("root");
	if (it != _data.end()) {
		_root = it->second;
		_data.erase(it);
	}
	else
		_root = "/var/www/html";
	it = _data.find("autoindex");
	if (it != _data.end()) {
		if (it->second == "on")
			_listing = true;
		_data.erase(it);
	}
	it = _data.find("index");
	if (it != _data.end()) {
		if (it->second.find('.') == NOT_FOUND)
			throw exc("invalid index\n");
		_index = it->second;
		_data.erase(it);
	}
	// printAll();
}

void server::checkValue() {
	std::string str;
	int num;
	for (std::vector<std::string>::iterator it = _listens.begin(); it != _listens.end(); it++) {
		int i = 0;
		if (it->find(':') != NOT_FOUND) {
			str = it->substr(0, it->find(':'));
			std::string str2 = it->substr(it->find(':') + 1);
			for (size_t j = 0; j < str2.length(); j++)
				if (!std::isdigit(str2[j]))
					throw exc("Error: invalid port: \"" + *it + "\"\n");
			num = std::atoi(str2.c_str());
			if (num > 65535 || num < 1)
				throw exc("Error: invalid port: " + *it + '\n');
			_ports.push_back(num);
			if (str.find('.') != NOT_FOUND) {
				for (; !str.empty(); i++) {
					std::string substr;
					if (str.find('.') != NOT_FOUND)
						substr = str.substr(0, str.find('.'));
					else
					 	substr = str;
					for (int j = 0; substr[j]; j++)
						if (!std::isdigit(substr[j]))
							throw exc("Error: invalid port: \"" + *it + "\"\n");
					int IPnum = std::atoi(substr.c_str());
					if (IPnum > 255)
						throw exc("Error: invalid port: \"" + *it + "\"\n");
					str.erase(0, substr.length() + 1);
				}
				if (i != 4)
					throw exc("Error: invalid port: \"" + *it + "\"\n");
			}
		}
		else {
			str = *it;
			for (size_t i = 0; i < str.length(); i++)
			{
				if (!std::isdigit(str[i]))
					throw exc("Error: invalid port: \"" + *it + "\"\n");
			}
			num = std::atoi(str.c_str());
			if (num > 65535 || num < 1)
				throw exc("Error: invalid port: " + *it + '\n');
			_ports.push_back(num);
		}
	}
	for (std::vector<std::string>::iterator it = _methods.begin(); it != _methods.end(); it++) {
		if (*it != "GET" &&	*it != "POST" && *it != "DELETE")
			throw exc("Error: method \"" + *it + "\" not valid\n");
	}
}

bool server::getListing() {
	return _listing;
}

std::string server::getIndex() {
	return _index;
}

location server::getLocation(std::string to_find) {
	location null;
	for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::string locationName;
		if (it->second.getRoot() != "")
			locationName = it->second.getRoot() + it->first;
		else
		 	locationName = it->first;
		if (locationName == to_find)
			return it->second;

	}
	return null;
}

bool server::getListen(std::string& to_find) {
	for (std::vector<std::string>::iterator it = _listens.begin(); it != _listens.end(); it++) {
		if (it->find(to_find) != NOT_FOUND)
			return true;
	}
	return false;
}

void server::checkPort(server to_check) {
	for (std::vector<std::string>::iterator it = _listens.begin(); it != _listens.end(); it++) {
		if (to_check.getListen(*it) == true)
			throw exc ("Error: port \""+ *it +"\" already used in another server\n");
	}
}

bool server::getServerName(std::string& to_find) {
	for (std::vector<std::string>::iterator it = _server_names.begin(); it != _server_names.end(); it++)
		if (*it == to_find)
			return true;
	return false;
}

bool server::checkLocation(std::string to_find) {
	for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::string locationName;
		if (it->second.getRoot() != "")
			locationName = it->second.getRoot() + it->first;
		else
		 	locationName = it->first;
		if (locationName == to_find) {
			return true;
		}
	}
	return false;
}

void server::startListens()
{
	int i = 0;
	_pollfd_size = _ports.size();
	_poll_fds = new struct pollfd[_pollfd_size];
	for (std::vector<int>::iterator it = _ports.begin(); it != _ports.end(); it++)
	{
		init(*it, i);
		i++;
	}
}

server::~server() {
	if (_poll_fds)
		delete[] _poll_fds;
}
