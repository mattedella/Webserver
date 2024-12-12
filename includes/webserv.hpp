
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
		size_t getSize() const;

		bool someInfo(std::string &str);

		void printMap();
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

	public:
		http();
		~http();
};


class location : public ABlock {

	public:
		location();
		~location();
};

class server : public ABlock {

	private:
		std::vector<int>		_sockets;
		std::map<std::string, location>	_locations;
		bool					_listing;

	public:

		int start();
		int accept();
		int stop();

		void addLocation(const std::string& Key, location loc);
		void printLocation();
		const std::map<std::string,std::string>::const_iterator findKey(const std::string& Key) const;

		server();
		~server();
};

class conf {

	private:
		std::vector<http>				_http;
		std::map<int,server>	_servers;
		// cgi					_cgi;

	public:
		void addServer(int nbrServer, const server& srv);
		void addHttp(const http& http);
		void printServer();
		// int reload(int port);
		conf();
		~conf();
};

void ParsFile(std::ifstream& myFile);



#endif