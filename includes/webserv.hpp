
#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <filesystem>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <map>
#include <vector>

class ABlock {

	protected:
		std::map<std::string, std::string> _data;
	
	public:
		ABlock();
		void initMap(std::string& str);
		virtual ~ABlock();
};

class cgi
{
	private:
		std::vector<int> fd;

	public:
		int excecute();
		cgi();
		~cgi();
};


class http : public ABlock {

	private:
		// std::map<int, std::string>	_error;
		// int							_maxBodySize;

	public:
		http();
		~http();
};


class location : public ABlock {

	private:
		// bool		_sublocation;
		std::string	_root;

	public:
		location();
		~location();
};

class server : public ABlock {

	private:
		std::vector<int>		_sockets;
		std::vector<location>	_locations;
		bool					_listing;

	public:

		int start();
		int accept();
		int stop();

		void addLocation(location loc);

		server();
		~server();
};

class conf {

	private:
		http				_http;
		std::vector<server>	_servers;
		cgi					_cgi;

	public:
		int reload(int port);
		conf();
		~conf();
};





#endif