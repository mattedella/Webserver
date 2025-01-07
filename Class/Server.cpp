
#include "../includes/webserv.hpp"
#include <cctype>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

location::location() : ABlock() {
	_methods = "";
	_bodySize = 30;
	_index  = "";
	_root = "";
}

void location::addVal() {
	for (std::map<std::string, std::string>::iterator it = _data.begin(); it != _data.end(); it++) {
		if (it->first == "root")
			_root = it->second;
		else if (it->first == "index")
			_index = it->second;
		else if (it->first == "client_max_body_size") {
			int size = std::atoi(it->second.c_str());
			if (it->second[it->second.length() - 1] != 'M' || size <= 0)
				throw exc("Error: body size value not valid\n");
			_bodySize = size;
		}
		else if (it->first == "dav_methods")
			_methods = it->second;
	}
	_data.erase("root");
	_data.erase("index");
	_data.erase("client_maxz_body_size");
	_data.erase("dav_methods");
}

void location::printVal() {
	std::cout << "-----LOCATION------\n";
	std::cout << "root: " << _root << '\n';
	std::cout << "methods: " << _methods << '\n';
	std::cout << "index: " << _index << '\n';
	std::cout << "bodySize: " << _bodySize << '\n';
}

location::~location() {}

server::server() : ABlock(), _listing(false) {
	_timeout = 60;
	_index = "index.html";
	_root = "/";
	_max_body_size = 30;
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
	if (_data.find("server_name") == _data.end())
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
			_server_names.push_back(it->second);
		}
		else if (it->first == "autoindex") {
			if (it->second == "on")
				_listing = true;
			else if (it->second != "off")
				throw exc("Invalid listing\n");
		}
	}
	_data.erase("listen");
	_data.erase("server_name");
	_data.erase("autoindex");
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

server::~server() {}


void server::printAll()
{
	std::cout << "PRINT ALL\n";
	std::cout << "Server\n";
	std::cout << "time out "<<_timeout << '\n';
	std::cout << "root "<<_root << '\n';
	std::cout << "max body size "<<_max_body_size << '\n';
	std::cout << "listing "<<_listing << '\n';
	std::cout << "index "<<_index << '\n';
	std::cout << "listen:\n";
	for (size_t i = 0; i < _listens.size(); i++)
		std::cout << _listens[i] << '\n';
	std::cout << "server name:\n";
	for (size_t i = 0; i < _server_names.size(); i++)
		std::cout << _server_names[i] << '\n';
	std::cout << "MAP\n";
	printMap();
	std::cout << "LOCATIONS\n";
	printLocation();
}

void server::addVal()
{
	std::multimap<std::string, std::string>::iterator it;

	while ((it = _data.find("listen")) != _data.end()) {
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
		_max_body_size = size;
		_data.erase(it);
	}
	it = _data.find("client_body_timeout");
	if (it != _data.end()) {
		int timeout = std::atoi(it->second.c_str());
			if (it->second[it->second.length() - 1] != 's' || timeout <= 0)
				throw exc("Error: body timeout value not valid\n");
		_timeout = timeout;
		_data.erase(it);
	}
	else
		_timeout = 60;
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
	printAll();
}

void server::checkValue()
{
	for (std::vector<std::string>::iterator it = _listens.begin(); it != _listens.end(); it++)
	{
		std::string str;
		int num;

		if (it->find(':') != NOT_FOUND)
		{
			str = it->substr(0, it->find(':'));
			std::string str2 = it->substr(it->find(':') + 1);
			for (size_t i = 0; i < str2.length(); i++)
				if (!std::isdigit(str2[i]))
					throw exc("Error: invalid port: " + *it + '\n');
			num = std::atoi(str2.c_str());
			if (num > 65535 || num < 1)
				throw exc("Error: invalid port: " + *it + '\n');
			if (str.find('.') != NOT_FOUND) {
				for (int i = 0; str[i]; i++) {
					std::string substr;
					if (str.find('.') != NOT_FOUND)
						substr = str.substr(0, str.find('.'));
					else
					 	substr = str;
					for (int j = 0; substr[j]; j++)
						if (!std::isdigit(substr[j]))
							throw exc("Error: invalid port: " + *it + '\n');
					int IPnum = std::atoi(substr.c_str());
					if (IPnum > 255)
						throw exc("Error: invalid port: " + *it + '\n');
					str.erase(0, substr.length() + 1);
				}
			}
		}
		else
		{
			str = *it;
			for (size_t i = 0; i < str.length(); i++)
			{
				if (!std::isdigit(str[i]))
					throw exc("Error: invalid port: " + *it + '\n');
			}
			num = std::atoi(str.c_str());
			if (num > 65535 || num < 1)
				throw exc("Error: invalid port: " + *it + '\n');
		}
	}
	
}