
#include "../includes/webserv.hpp"
#include <utility>

server::server() : ABlock(), _listing(false) {}

size_t server::getLocationSize() const
{
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
		it->second.printMap();
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

location::location() : ABlock() {}

location::~location() {}

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
		_max_body_size = it->second;
		_data.erase(it);
	}
	it = _data.find("client_body_timeout");
	if (it != _data.end()) {
		_timeout = it->second;
		_data.erase(it);
	}
	else
		_timeout = "30";
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
		_index = it->second;
		_data.erase(it);
	}
	printAll();
}