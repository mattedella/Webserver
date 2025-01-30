
#ifndef CONF_HPP
# define CONF_HPP

#include "Ablock.hpp"
#include "Http.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <csignal>

class conf {

	private:
		std::vector<http>		_http;
		std::map<int,server>	_servers;
		std::string				_fullPath;
		bool					_listing;

	public:
		conf();

		void		addKey();
		void		addHttp(const http& http);
		void		addServer(int nbrServer, const server& srv);

		void		printHttp();
		void		printServer();

		bool		getListing();
		std::string	getFullPath();
		server getServer(std::string port);
		server		getServer(int nbrServer);
		std::map<int, server>& getMapServer();
		location	getLocation(std::string to_find, int nbrServer);
		std::string	getErrorPage(int error, int nbrServer, location location);

		void		check();
		void		addHost();
		void		checkRequest(Request* req);

		void	run();

		~conf();
};



# endif