
#ifndef CONF_HPP
# define CONF_HPP

#include "Ablock.hpp"
#include "Http.hpp"
#include "Server.hpp"

class conf {

	private:
		std::vector<http>		_http;
		std::map<int,server>	_servers;

	public:
		void addServer(int nbrServer, const server& srv);
		void addKey();
		void addHttp(const http& http);
		void printServer();
		void printHttp();
		void check();
		// int reload(int port);


		std::map<int, server> getMapServer();
		server getServer(std::string port);
		void	run();
		conf();
		~conf();
};

# endif