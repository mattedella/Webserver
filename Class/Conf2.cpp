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
	std::map<int, server>::iterator it = _servers.begin();
	while (true) {
		it->second.s_run();
    }
}