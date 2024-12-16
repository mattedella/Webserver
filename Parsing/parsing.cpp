
#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <string>
#include <variant>

void checkLine(const std::string &new_line, int &brk)
{
	if (new_line.find('#') != std::string::npos)
		return;
	if (new_line.size() != 0)
	{
		char last = new_line[new_line.size() - 1];

		if (last != ';' && last != '{' && last != '}' && !std::isspace(last))
			throw exc("Error: " + new_line + "\n");
	}
	if (new_line.find('{') != std::string::npos)
				brk++;
	if (new_line.find('}') != std::string::npos)
	{
		if (brk <= 0)
			throw exc(("\nError: " + new_line + "! \n"));
		brk--;
	}
}


void ParsFile(std::ifstream& myFile) {

	std::string new_line;
	http		httpBlock;
	server		serverBlock;
	location	locationBlock;
	conf		c;
	int brk = 0;
	int cOpen = 0;
	int	nbrServer = 0;
	// int nbrLoc = 0;
	try 
	{
		while (std::getline(myFile, new_line)) {
			
			checkLine(new_line, brk);
			if (new_line.find('#') != std::string::npos && new_line.find('#') < new_line.find(';'))
				continue ;
			if (new_line.find("http") != std::string::npos) {
				while (std::getline(myFile, new_line)) {
					checkLine(new_line, brk);
					if (new_line.find('#') != std::string::npos)
						continue ;
					if (new_line.find("server") != std::string::npos
						|| new_line.find('}') != std::string::npos)
						break ;
					httpBlock.initMap(new_line);
				}
				httpBlock.initVector();
			}
			if (new_line.find("server") != std::string::npos) {
				cOpen = 1;
				serverBlock = server();
				while (std::getline(myFile, new_line)) {
					checkLine(new_line, brk);
					if (new_line.find('#') != std::string::npos)
						continue ;
					if (new_line.find("location") != std::string::npos
						|| (new_line.find('}') != std::string::npos && cOpen == 1))
						break ;
					serverBlock.initMap(new_line);
				}
				serverBlock.initVector();
				nbrServer++;
				serverBlock.printLocation();
			}
			if (new_line.find("location") != std::string::npos) {
				cOpen = 1;
				std::string root = new_line.substr(new_line.find(' '));
				locationBlock = location();
				while (cOpen == 1) {
					while (std::getline(myFile, new_line)) {
						checkLine(new_line, brk);
						if (new_line.find('#') != std::string::npos)
							continue ;
						if (new_line.find("}") != std::string::npos) {
							cOpen = 0;
							break ;
						}
						locationBlock.initMap(new_line);

					}
					serverBlock.addLocation(root, locationBlock);
				}
			}
			c.addServer(nbrServer, serverBlock);
			c.addHttp(httpBlock);
		}
		c.printServer();
		c.check();
		if (brk != 0)
			throw exc("Error: unclosed brackets!\n");
	}
	catch (std::exception &e)
	{
		std::cout<<e.what();
	}
}