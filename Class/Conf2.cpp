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
	// char e;
	if (sig == SIGINT)
	{
		// std::cout<< "\nSIGINT (CTRL + C) recived, do you want to exit? (y/n)\n";
		// std::cin>>e;
		// if (e == 'y' || e == 'Y')
			exit(1);
		// else if (e == 'n' || e == 'N')
		//  	return ;
		// else
		//  	std::cout<< "\nYou are stupid, you had only two options (y/n), then I will continue\n"; 
	}

}

void conf::run()
{
	int i = -1;
	
	std::signal(SIGINT, conf::handleSignal);
	std::map<int, server>::iterator it = _servers.begin();
	Request *req = new Request();
	while (i < 0 && _running == true) {
		//it->second.printFdsVect();
		it->second.s_run(this, req);
    }
}