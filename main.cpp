#include "includes/webserv.hpp"
#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>

int main(int ac, char **av) {
	
	if (ac != 2)
		return (std::cerr<<"Error: wrong number of arguments\n", 1);
	
	std::string new_line;
	// std::string parsLine;
	std::ifstream myfile;
	http		http;
	server		server;
	location	location;
	conf		c;
	myfile.open(av[1]);
	if (!myfile.is_open())
		return (std::cerr << "Error: file corrupted or inexistent\n", 1);
	
	while (std::getline(myfile, new_line)) {
		if (new_line.find('#') != std::string::npos)
			continue ;
		if (new_line.find("http") != std::string::npos) {
			while (std::getline(myfile, new_line)) {
				if (new_line.find("server") != std::string::npos)
					break ;
				http.initMap(new_line);
			}
		}
		if (new_line.find("server") != std::string::npos) {
			while (std::getline(myfile, new_line)) {
				if (new_line.find("location") != std::string::npos)
					break ;
				server.initMap(new_line);
			}
			c.addServer(server);

		}
		if (new_line.find("location") != std::string::npos) {
			while (std::getline(myfile, new_line)) {
				if (new_line.find("server") != std::string::npos)
					break ;
				if (new_line.find("location") != std::string::npos)
					break ;
				location.initMap(new_line);
			}
			server.addLocation(location);
		}
	}
	std::cout<<"\n\nPRINT\n\nhttp\n";
	http.printMap();
	std::cout<<"\nserver\n";
	// server.printMap();
	c.printServer();
	std::cout<<"\nlocation\n";
	location.printMap();
	myfile.close();
	return 0;
}