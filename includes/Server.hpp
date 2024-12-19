#ifndef SERVER_HPP
# define SERVER_HPP

#include "Ablock.hpp"
#include <string>

class ABlock;

class location : public ABlock {

	public:
		location();
		~location();
};

class server : public ABlock {

	private:
		std::string						_timeout;
		std::string						_index;
		std::string						_root;
		std::string						_max_body_size;
		bool							_listing;
		std::vector<int>				_sockets;
		std::vector<std::string>		_listens;
		std::vector<std::string>		_server_names;
		std::map<std::string, location>	_locations;

	public:

		int stop();
		void printAll();
		int start();
		int accept();
		void initVector();
		void printLocation();
		size_t getLocationSize() const;
		void addLocation(const std::string& Key, location loc);

		bool		getListing();
		location	getLocation(std::string& to_find);
		std::string getListen(std::string& to_compare);
		std::string getServerName(std::string& to_compare);

		void addVal();

		void checkValue();

		server();
		~server();
};

#endif