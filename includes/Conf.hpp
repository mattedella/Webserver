
#ifndef CONF_HPP
# define CONF_HPP

#include "Ablock.hpp"
#include "Http.hpp"
#include "Server.hpp"

class conf {

	private:
		std::vector<http>				_http;
		std::map<int,server>	_servers;
		// cgi					_cgi;

	public:
		void addServer(int nbrServer, const server& srv);
		void addHttp(const http& http);
		void printServer();
		void check();
		// int reload(int port);
		conf();
		~conf();
};

# endif