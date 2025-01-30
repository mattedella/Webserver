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
	int i = -1;
	
	std::signal(SIGINT, handleSignal);
	std::map<int, server>::iterator it = _servers.begin();
	Request *req = new Request();
	while (i < 0) {
		if (Quit == true)
			break ;
		it->second.s_run(this, req);
    }
	it = _servers.begin();
	for (; it != _servers.end(); it++)
		it->second.closeSocket();
	delete req;
}