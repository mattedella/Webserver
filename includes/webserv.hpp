
#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Http.hpp"
#include "Server.hpp"
#include "Conf.hpp"
#include "Ablock.hpp"
#include "Exc.hpp"
#include <cerrno>
#include <string>

# define	NOT_FOUND std::string::npos
# define	BLUE "\033[34m"
# define	RED "\033[31m"
# define	RESET "\033[0m"
# define	GREEN "\033[32m"
# define	YELLOW "\033[33m"
class cgi
{
	private:
		std::vector<int> fd;

	public:
		cgi();

		int excecute();
		
		~cgi();
};

conf *ParsConfFile(std::vector<std::string> config_content);
void ParsHttp(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);
void ParsServer(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);
void ParsLocation(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);


#endif