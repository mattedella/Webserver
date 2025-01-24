
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
		bool					_running;

	public:
		void		addServer(int nbrServer, const server& srv);
		void		addKey();
		void		addHttp(const http& http);
		void		printServer();
		void		printHttp();
		void		check();
		std::string	getErrorPage(int error, int nbrServer, location location);
		std::string	getFullPath();
		location	getLocation(std::string to_find, int nbrServer);
		server		getServer(int nbrServer);

		void		checkRequest(Request* req);
		static void	handleSignal(int sig);

// int			reload(int port);
		std::map<int, server>& getMapServer();
		server getServer(std::string port);
		void	run();
		conf();
		~conf();
};

# endif