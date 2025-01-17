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
    bool running = true;

    while(running) {
        for (std::map<int, server>::iterator it = _servers.begin(); 
             it != _servers.end(); ++it) 
        {
            Request *req = new Request();
            // Run one iteration of server event loop
            it->second.s_run(*this, req);
            delete req;
        }
    }
}