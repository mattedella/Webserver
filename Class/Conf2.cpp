#include "../includes/webserv.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>


void conf::run()
{
	int i = -1;
	std::map<int, server>::iterator it = _servers.begin();
	Request *req = new Request();
	while (i < 0) {
		//it->second.printFdsVect();
		it->second.s_run(*this, req);
    }
}