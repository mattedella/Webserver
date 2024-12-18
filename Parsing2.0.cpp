
// #include "includes/webserv.hpp"
// #include <cctype>
// #include <cstdio>
// #include <exception>
// #include <fstream>
// #include <sstream>
// #include <vector>

// bool checkLine(const std::string &new_line, int &brk)
// {
// 	int i = 0;
// 	if (new_line.size() != 0)
// 	{
// 		if (!new_line[0] || new_line[0] == '#')
// 			return true;
// 		while(new_line[i] && new_line[i] != ';' && new_line[i] != '{' && new_line[i] != '}')
// 		{
// 			if (new_line[i] == '#')
// 				throw exc("Error: " + new_line + "\n");
// 			i++;
// 		}
// 			if (new_line[i] =='{')
// 				brk++;
// 			else if (new_line[i] =='}')
// 			{
// 				brk--;
// 				if (brk < 0)
// 					throw exc("Error: " + new_line + "\n");
// 			}
// 		if (!new_line[i])
// 			throw exc("Error: " + new_line + "\n");
// 		else
// 			i++;		
// 		while(std::isspace(new_line[i]))
// 			i++;
// 		if (new_line[i] == '#')
// 			return false;
// 		if (new_line[i] && new_line[i] != '#')
// 			throw exc("Error: " + new_line + "\n");			
// 	}
// 	return false;
// }

// void ParsHttp(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end, http& HttpBlock) {
// 	it++;
// 	for (; it != end; ++it) {
// 		std::string line = *it;
// 		if (line.find("server") != std::string::npos || line.find('}') != std::string::npos) {
// 			it--;
// 			break ;
// 		}
// 		HttpBlock.initMap(line);
// 	}
// 	HttpBlock.initVector();
// }

// void ParsServer(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end, server& ServerBlock) {
// 	it++;
// 	ServerBlock = server();
// 	for (; it != end; ++it) {
// 		std::string line = *it;
// 		if (line.find("location") != std::string::npos || line.find('}') != std::string::npos) {
// 			it--;
// 			break ;
// 		}
// 		ServerBlock.initMap(line);
// 	}
// 	ServerBlock.initVector();
// }

// void ParsLocation(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end, server& ServerBlock, location& LocationBlock) {
// 	std::string path;
// 	LocationBlock = location();
// 	if (it->find('/') != std::string::npos)
// 		path = it->substr(it->find('/'), (it->find('{') - it->find('/')) - 1);
// 	else if (it->find('\\') != std::string::npos)
// 		path = it->substr(it->find('\\'), (it->find('{') - it->find('\\')) - 1);\
// 	else
// 	 	throw exc("Error: configuration of " + *it + "\n");
// 	while (std::isspace(path[0]))
// 		path.erase(0, 1);
// 	it++;
// 	for (; it != end; ++it) {
// 		std::string line = *it;
// 		if (line.find("server") != std::string::npos || line.find('}') != std::string::npos) {
// 			it--;
// 			break ;
// 		}
// 		// std::cout << "Line to add: " << line << '\n';
// 		LocationBlock.initMap(line);
// 	}
// 	ServerBlock.addLocation(path, LocationBlock);
// }

// void ParsConfFile(std::vector<std::string> config_content) {
// 	int 		brk = 0, nbrServer = 0;
// 	server		ServerBlock;
// 	http		HttpBlock;
// 	location	LocationBlock;
// 	conf		ConfigurationBlock;
// 	try {
// 		for (std::vector<std::string>::iterator it = config_content.begin(); it != config_content.end(); it++) {
// 			std::string line = *it, subline;
// 			if (checkLine(line, brk) == true)
// 				continue ;
// 			if (line.find('{') != std::string::npos)
// 				subline = line.substr(0, line.find('{'));
// 			else if (line.find(';') != std::string::npos)
// 				subline = line.substr(0, line.find(';'));
// 			else if (line.find('}') != std::string::npos)
// 				subline = line.substr(0, line.find(' ') + 1);
// 			if (subline.empty() || subline[0] == '#')
// 				continue ;
// 			int i = subline.length() - 1;
// 			while ( i >= 0 && std::isspace(subline[i])) {
// 				subline.erase(i, 1);
// 				i--;
// 			}
// 			if (subline == "http") {
// 				ParsHttp(it, config_content.end(), HttpBlock);
// 				ConfigurationBlock.addHttp(HttpBlock);
// 			}
// 			else if (subline == "server") {
// 				ParsServer(it, config_content.end(), ServerBlock);
// 				nbrServer++;
// 			}
// 			else if (subline.find("location") != std::string::npos) {
// 				std::string comp;
// 				if (subline.find(' ') != std::string::npos)
// 					comp = subline.substr(0, subline.find(' '));
// 				else if (subline.find('\t') != std::string::npos)
// 					comp = subline.substr(0, subline.find('\t'));
// 				int i = comp.length() - 1;
// 				while (std::isspace(comp[i]))
// 					comp.erase(i, 1);
// 				if (comp == "location") {
// 					ParsLocation(it, config_content.end(), ServerBlock, LocationBlock);
// 					ConfigurationBlock.addServer(nbrServer, ServerBlock);
// 				}
// 			}
// 		}
// 	ConfigurationBlock.check();
// 	ConfigurationBlock.printServer();
// 	}
// 	catch (std::exception& e) {
// 		std::cerr << e.what();
// 	}
// }

// int main (int argc, char **argv) {

// 	if (argc != 2) {
// 		std::cerr << "Error: Wrong number of argumets\n";
// 		std::cerr << "Use: " << argv[0] << " <configuration_file>\n";
// 		return 1;
// 	}
// 	// std::ifstream myFile;
// 	// myFile.open(argv[1]);
// 	// if (!myFile.is_open())
// 	// 	return (std::cerr << "Error: file corrupted or inexistent\n", 1);
// 	// std::string new_line;
// 	// const char* config_filename = argv[1];
//     std::ifstream config_file(argv[1]);
//     if (!config_file.is_open()) {
//         std::cerr << "Error: Could not open the file " << argv[1] << std::endl;
//         return 1;
//     }
// 	std::string newline;
//     std::vector<std::string> conf_line;
// 	while (std::getline(config_file, newline)) {
// 		while (std::isspace(newline[0]))
// 			newline.erase(0, 1);
// 		conf_line.push_back(newline);
// 	}
//     ParsConfFile(conf_line);
//     // for (size_t i = 0; i < conf_line.size(); i++)
// 	// 	std::cout << "riga" << i << ": " << conf_line[i] << std::endl;

// }