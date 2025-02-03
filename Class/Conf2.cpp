#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <csignal>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

void handleSignal(int sig)
{
	if (sig == SIGINT)
	{
		Quit = true;
		std::cout<<"\nCTRL+C pressed, closing connection!\n";
	}

}

void conf::run()
{
	
	signal(SIGINT, handleSignal);
	Request *req = new Request();
	while(Quit == false) {
		for (std::map<int, server>::iterator it = _servers.begin(); 
			it != _servers.end(); ++it) 
		{
			if (Quit == true)
				break ;
			it->second.s_run(this, req);
		}
	}
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->second.closeSocket();
	delete req;
}