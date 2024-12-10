#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <map>
#include <vector>

// class mySocket
// {
// 	struct sockaddr_in _addr;

// };

class cgi
{
	private:
		std::vector<int> fd;
	public:
		int excecute();
		cgi();
		~cgi();
};


class http
{
	private:
		std::map<int, std::string> _error;
		std::map<std::string, std::string> _data;
		int _maxBodySize;

	public:
		http();
		~http();
};

class location
{
	private:
		std::map<std::string, std::string> _root;
	public:
		location();
		~location();
};

class server
{
	private:
		std::vector<int> _sockets;
		std::map<std::string, std::string> _data;
		std::vector<location> _locations;
		bool _listing;
	public:

		int start();
		int accept();
		int stop();

		server();
		~server();
};

class conf
{
	private:
		http _http;
		std::vector<server> _servers;
		cgi _cgi;
	public:
		int reload(int port);
		conf();
		~conf();
};





#endif