
#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <string>
#include <variant>

bool checkLine(const std::string &new_line, int &brk)
{
	int i = 0;
	if (new_line.size() != 0)
	{
		while(std::isspace(new_line[i]))
			i++;
		if (!new_line[i] || new_line[i] == '#')
			return true;
		while(new_line[i] && new_line[i] != ';' && new_line[i] != '{' && new_line[i] != '}')
		{
			if (new_line[i] == '#')
				throw exc("Error: " + new_line + "\n");
			i++;
		}
			if (new_line[i] =='{')
				brk++;
			else if (new_line[i] =='}')
			{
				brk--;
				if (brk < 0)
					throw exc("Error: " + new_line + "\n");
			}
		if (!new_line[i])
			throw exc("Error: " + new_line + "\n");
		else
			i++;		
		while(std::isspace(new_line[i]))
			i++;
		if (new_line[i] == '#')
			return false;
		if (new_line[i] && new_line[i] != '#')
			throw exc("Error: " + new_line + "\n");			
	}
	return false;
}

void initLocation(std::string& new_line, std::ifstream& myFile, location& locationBlock, int& brk) {
	int cOpen = 1;
	locationBlock = location();
	while (cOpen == 1) {
		while (std::getline(myFile, new_line)) {
			if (checkLine(new_line, brk) == true)
				continue ;
			if (new_line.find("}") != std::string::npos) {
				cOpen = 0;
				break ;
			}
			locationBlock.initMap(new_line);
		}
	}
}

void initServer(std::string& new_line,  std::ifstream& myFile, server& serverBlock, int& brk, int& nbrServer) {
	int cOpen = 1;
	serverBlock = server();
	while (std::getline(myFile, new_line)) {
		if (checkLine(new_line, brk) == true)
				continue ;
		if (new_line.find("location") != std::string::npos
			|| (new_line.find('}') != std::string::npos && cOpen == 1))
			break ;
		serverBlock.initMap(new_line);
	}
	serverBlock.initVector();
	nbrServer++;
}

void initHttp(std::string& new_line, std::ifstream& myFile, int& brk, http& httpBlock) {
	while (std::getline(myFile, new_line)) {
	if (checkLine(new_line, brk) == true)
				continue ;
	if (new_line.find("server") != std::string::npos
		|| new_line.find('}') != std::string::npos)
		break ;
	httpBlock.initMap(new_line);
	}
	httpBlock.initVector();
}

void ParsFile(std::ifstream& myFile) {

	std::string new_line;
	http		httpBlock;
	server		serverBlock;
	location	locationBlock;
	conf		c;
	int brk = 0;
	int	nbrServer = 0;
	try 
	{
		while (std::getline(myFile, new_line)) {
			if (checkLine(new_line, brk) == true)
				continue ;
			if (new_line.find("http") != std::string::npos) {
				initHttp(new_line, myFile, brk, httpBlock);
			}
			if (new_line.find("server") != std::string::npos)
				initServer(new_line, myFile, serverBlock, brk, nbrServer);
			if (new_line.find("location") != std::string::npos) {
				std::string root = new_line.substr(new_line.find(' '));
				initLocation(new_line, myFile, locationBlock, brk);
				serverBlock.addLocation(root, locationBlock);
				}
			c.addServer(nbrServer, serverBlock);
			c.addHttp(httpBlock);
		}
		c.check();
		c.printServer();
		if (brk != 0)
			throw exc("Error: unclosed brackets!\n");
	}
	catch (std::exception &e)
	{
		std::cout<<e.what();
	}
}