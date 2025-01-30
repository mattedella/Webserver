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
	}

}

void conf::run()
{
	bool running = true;

	Request *req = new Request();
	while(running) {
		for (std::map<int, server>::iterator it = _servers.begin(); 
			it != _servers.end(); ++it) 
		{
			// Run one iteration of server event loop
			it->second.s_run(this, req);
		}
	}
	for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->second.closeSocket();
	delete req;
}