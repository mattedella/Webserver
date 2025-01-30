
#include "../includes/webserv.hpp"


bool checkLine(const std::string &new_line, int &brk)
{
	int i = 0;
	if (new_line.size() != 0) {
		if (!new_line[0] || new_line[0] == '#')
			return true;
		while(new_line[i] && new_line[i] != ';' && new_line[i] != '{' && new_line[i] != '}') {
			if (new_line[i] == '#')
				throw exc("Error: " + new_line + "\n");
			i++;
		}
		if (new_line[i] =='{')
			brk++;
		else if (new_line[i] =='}') {
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

void ParsHttp(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end, http& HttpBlock) {
	it++;
	for (; it != end; ++it) {
		if (it->empty() == false && it->find('#') == NOT_FOUND && it->find('}') == NOT_FOUND && it->find('{') == NOT_FOUND && it->find(';') == NOT_FOUND)
			throw exc("Error: \"" + *it + "\" not valid\n");
		std::string line = *it;
		if (line.find("server") != NOT_FOUND || line.find('}') != NOT_FOUND) {
			it--;
			break ;
		}
		if (line.find('#', 0) != NOT_FOUND)
			continue;
		if (line.find("http") != NOT_FOUND)
			throw exc("Error: invalid token inside http\n");
		HttpBlock.initMap(line);
	}
	HttpBlock.initVector();
}

void ParsServer(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end, server& ServerBlock) {
	it++;
	ServerBlock = server();
	for (; it != end; ++it) {
		if (it->empty() == false && it->find('#') == NOT_FOUND && it->find('}') == NOT_FOUND && it->find('{') == NOT_FOUND && it->find(';') == NOT_FOUND)
			throw exc("Error: \"" + *it + "\" not valid\n");
		std::string line = *it;
		if (line.find("location") != NOT_FOUND || line.find('}') != NOT_FOUND) {
			it--;
			break ;
		}
		if (line.find('#', 0) != NOT_FOUND)
			continue;
		if (line.find("server") != NOT_FOUND && line.find("server_name") == NOT_FOUND)
			throw exc("Error: invalid token inside server " + *it + '\n');
		ServerBlock.initMap(line);
	}
	ServerBlock.initVector();
}

void ParsLocation(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end, server& ServerBlock, location& LocationBlock) {
	std::string path;
	LocationBlock = location();
	if (it->find('/') != NOT_FOUND)
		path = it->substr(it->find('/'), (it->find('{') - it->find('/')) - 1);
	else
	 	throw exc("Error: location not valid " + *it + "\n");
	while (std::isspace(path[0]))
		path.erase(0, 1);
	it++;
	for (; it != end; ++it) {
		if (it->empty() == false && it->find('#') == NOT_FOUND && it->find('}') == NOT_FOUND && it->find('{') == NOT_FOUND && it->find(';') == NOT_FOUND)
			throw exc("Error: \"" + *it + "\" not valid\n");
		std::string line = *it;
		if (line.find('}') != NOT_FOUND) {
			it--;
			break ;
		}
		if (line.find('#', 0) != NOT_FOUND)
			continue;
		if (line.find("server") != NOT_FOUND || line.find("location") != NOT_FOUND
			|| line.find("http") != NOT_FOUND)
			throw exc("Error: invalid token inside location\n");
		LocationBlock.initMap(line);
	}
	LocationBlock.addVal();
	ServerBlock.addLocation(path, LocationBlock);
}

conf *ParsConfFile(std::vector<std::string> config_content) {
	int 		brk = 0, nbrServer = 0;
	server		ServerBlock;
	http		HttpBlock;
	location	LocationBlock;
	conf		*ConfigurationBlock = new conf();
	for (std::vector<std::string>::iterator it = config_content.begin(); it != config_content.end(); it++) {
		std::string line = *it, subline;
		if (checkLine(line, brk) == true)
			continue ;
		if (line.find('{') != NOT_FOUND)
			subline = line.substr(0, line.find('{'));
		else if (line.find(';') != NOT_FOUND)
			subline = line.substr(0, line.find(';'));
		else if (line.find('}') != NOT_FOUND)
			subline = line.substr(0, line.find(' ') + 1);
		if (subline.empty() || subline[0] == '#')
			continue ;
		int i = subline.length() - 1;
		while ( i >= 0 && std::isspace(subline[i])) {
			subline.erase(i, 1);
			i--;
		}
		if (subline == "http") {
			ParsHttp(it, config_content.end(), HttpBlock);
			ConfigurationBlock->addHttp(HttpBlock);
		}
		else if (subline == "server") {
			ParsServer(it, config_content.end(), ServerBlock);
			nbrServer++;
			ConfigurationBlock->addServer(nbrServer, ServerBlock);
		}
		else if (subline.find("location") != NOT_FOUND) {
			std::string comp;
			if (subline.find(' ') != NOT_FOUND)
				comp = subline.substr(0, subline.find(' '));
			else if (subline.find('\t') != NOT_FOUND)
				comp = subline.substr(0, subline.find('\t'));
			int i = comp.length() - 1;
			while (std::isspace(comp[i])) {
				comp.erase(i, 1);
				i--;
			}
			if (comp == "location") {
				ParsLocation(it, config_content.end(), ServerBlock, LocationBlock);
				ConfigurationBlock->addServer(nbrServer, ServerBlock);
			}
			else
				throw exc("Error: invalid token: " + line + '\n');
		}
	}
	ConfigurationBlock->addKey();
	ConfigurationBlock->check();
	ConfigurationBlock->addHost();
	return ConfigurationBlock;
}
