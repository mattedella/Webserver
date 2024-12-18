
#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Http.hpp"
#include "Server.hpp"
#include "Conf.hpp"
#include "Ablock.hpp"
#include "Exc.hpp"

# define  NOT_FOUND std::string::npos

class cgi
{
	private:
		std::vector<int> fd;

	public:
		int excecute();
		cgi();
		~cgi();
};

void ParsConfFile(std::vector<std::string> config_content);
void ParsHttp(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);
void ParsServer(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);
void ParsLocation(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);



#endif