#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <csignal>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

void conf::handleSignal(int sig)
{
	if (sig == SIGINT) {
			
		// TODO: liberazione memoria con garbage collector;
			exit(1);
	}

}

void conf::run()
{
	int i = -1;
	
	std::signal(SIGINT, conf::handleSignal);
	std::map<int, server>::iterator it = _servers.begin();
	Request *req = new Request();
	while (i < 0 && _running == true) {
		it->second.s_run(this, req);
    }
}