
#include "../includes/webserv.hpp"
#include <vector>

server::server() : ABlock(), _listing(false) {}

size_t server::checkLocation() const
{
	return _locations.size();
}

void server::addLocation(const std::string& Key, location loc) {
	_locations.insert(std::make_pair(Key, loc));
}

void server::initVector() {
	if (_data.find("listen") == _data.end())
		throw exc("Error: no key listen found\n");
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
	std::cout << "listen :\n";
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