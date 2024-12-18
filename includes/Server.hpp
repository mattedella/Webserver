#ifndef SERVER_HPP
# define SERVER_HPP

#include "Ablock.hpp"

class ABlock;

class location : public ABlock {

	public:
		location();
		~location();
};

class server : public ABlock {

	private:
		std::vector<std::string>		_listens;
		std::vector<std::string>		_server_names;
		std::vector<int>		_sockets;
		std::map<std::string, location>	_locations;
		bool					_listing;

	public:

		int start();
		int accept();
		int stop();

		size_t checkLocation() const;
		void initVector();
		void addLocation(const std::string& Key, location loc);
		void printLocation();
		std::string getListen(std::string& to_compare);
		std::string getServerName(std::string& to_compare);
		location	getLocation(std::string& to_find);
		bool		getListing();
		server();
		~server();
};

#endif