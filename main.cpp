#include "includes/Server.hpp"
#include "includes/webserv.hpp"
#include <iostream>
#include <map>

int StatusCode = 0;
bool Quit = false;

int main (int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "\033[31m" << "Error: Wrong number of argumets\n";
		std::cerr << "Use: " << argv[0] << " <configuration_file>\n" << "\033[0m";
		return 1;
	}
	std::ifstream config_file(argv[1]);
	if (!config_file.is_open()) {
		std::cerr << "\033[31m" <<"Error: Could not open the file " << argv[1] << "\033[0m"<< std::endl;
		return 1;
	}
	std::string newline;
	std::vector<std::string> conf_line;
	conf *c = NULL;
	while (std::getline(config_file, newline)) {
		while (std::isspace(newline[0]))
			newline.erase(0, 1);
		conf_line.push_back(newline);
	}
	try {
		c = ParsConfFile(conf_line);
		std::map<int, server>& startListen = c->getMapServer();
		for (std::map<int, server>::iterator it = startListen.begin(); it != startListen.end(); it++)
		{
			std::cout<< BLUE << "Server " <<it->first << ": starting\n" << RESET;
			it->second.startListens();
			it->second.starting();
		}
		c->run();
	}
	catch (std::exception& e) {
		std::cerr << RED << e.what() << RESET;
	}
	delete c;
}
