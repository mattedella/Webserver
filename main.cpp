#include "includes/webserv.hpp"
#include <fstream>
#include <ostream>
#include <string>

int main(int ac, char **av) {
	
	if (ac != 2)
		return (std::cerr<<"Error: wrong number of arguments\n", 1);
	
	std::string new_line;
	std::string parsLine;
	std::ifstream myfile;
	http		http;
	// server		server;
	// cgi			cgi;

	myfile.open(av[1]);
	if (!myfile.is_open())
		return (std::cerr << "Error: file corrupted or inexistent\n", 1);
	
	while (std::getline(myfile, new_line)) {
		std::cout << "string= " << new_line << '\n';
		if (new_line.find('#') != std::string::npos)
			continue ;
		if (new_line.find("http") != std::string::npos) {
			while (std::getline(myfile, parsLine)) {
				if (parsLine.find("server") != std::string::npos)
					break ;
				http.initMap(parsLine);
			}
		}
	}
	myfile.close();
	return 0;
}