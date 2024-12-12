
#include "../includes/webserv.hpp"
#include <fstream>
#include <string>

void ParsFile(std::ifstream& myFile) {

	std::string new_line;
	http		httpBlock;
	server		serverBlock;
	location	locationBlock;
	conf		c;
	int cOpen = 0;
	int	nbrServer = 0;

	while (std::getline(myFile, new_line)) {
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
			serverBlock = server();
			while (std::getline(myFile, new_line)) {
				if (new_line.find("location") != std::string::npos)
					break ;
				serverBlock.initMap(new_line);
			}
			nbrServer++;
		}
		if (new_line.find("location") != std::string::npos) {
			cOpen = 1;
			std::string root = new_line.substr(new_line.find(' '));
			locationBlock = location();
			while (cOpen == 1) {
				while (std::getline(myFile, new_line)) {
					if (new_line.find("}") != std::string::npos) {
						cOpen = 0;
						break ;
					}
					locationBlock.initMap(new_line);

				}
				std::cout << "location add\n";
				serverBlock.addLocation(root, locationBlock);
				serverBlock.printLocation();		
			}
		}
		c.addServer(nbrServer, serverBlock);
		c.addHttp(httpBlock);
	}
	std::cout<<"\nPRINT\n===nhttp===\n";
	httpBlock.printMap();
	std::cout<<"\n===server===\n";
	c.printServer();
}