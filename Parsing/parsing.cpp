
#include "../includes/webserv.hpp"
#include <fstream>

void ParsFile(std::ifstream& myFile) {

	std::string new_line;
	http		httpBlock;
	server		serverBlock;
	location	locationBlock;
	conf		c;

	while (std::getline(myFile, new_line)) {
		serverBlock = server();
		if (new_line.find('#') != std::string::npos)
			continue ;
		if (new_line.find("http") != std::string::npos) {
			while (std::getline(myFile, new_line)) {
				if (new_line.find("server") != std::string::npos)
					break ;
				httpBlock.initMap(new_line);
			}
		}
		if (new_line.find("server") != std::string::npos) {
			while (std::getline(myFile, new_line)) {
				if (new_line.find("location") != std::string::npos)
					break ;
				serverBlock.initMap(new_line);
			}
		}
		if (new_line.find("location") != std::string::npos) {
			locationBlock = location();
			std::string root = new_line.substr(new_line.find(' '));
			while (std::getline(myFile, new_line)) {
				if (new_line.find("server") != std::string::npos)
					break ;
				if (new_line.find("location") != std::string::npos) {
					serverBlock.addLocation(root, locationBlock);
					root = new_line.substr(new_line.find(' '));
					locationBlock = location();
				}
				locationBlock.initMap(new_line);
			}
		}
		c.addServer(serverBlock);
	}
	std::cout<<"\nPRINT\n\nhttp---\n";
	httpBlock.printMap();
	std::cout<<"\nserver---\n";
	c.printServer();
	std::cout<<"\nlocation\n";
	locationBlock.printMap();
}
