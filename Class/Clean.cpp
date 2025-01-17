#include <signal.h>
#include <vector>
#include <map>
#include "../includes/webserv.hpp"
static conf* g_conf = NULL;

void cleanupResources() {
    if (!g_conf) {
        return;
    }

    std::map<int, server>& servers = g_conf->getMapServer();
    for (std::map<int, server>::iterator srv_it = servers.begin(); 
         srv_it != servers.end(); ++srv_it) 
    {
        server& current_server = srv_it->second;
    
        const std::vector<struct pollfd>& poll_fds = current_server.getPollFds();
        
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            bool is_server_socket = false;
            for (size_t j = 0; j < current_server.getServerSockets().size(); ++j) {
                if (poll_fds[i].fd == current_server.getServerSockets()[j].fd) {
                    is_server_socket = true;
                    break;
                }
            }
            
            if (!is_server_socket) {
                current_server.close_connection(i);
                --i;
            }
        }
    
        for (size_t i = 0; i < current_server.getServerSockets().size(); ++i) {
            close(current_server.getServerSockets()[i].fd);
        }
        
        current_server.getServerSockets().clear();
        current_server.getPollFds().clear();
        current_server.getClientBuffers().clear();
        current_server.getClientResponses().clear();
    }

    delete g_conf;
    g_conf = NULL;
}

void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\n" << RED << "CTRL + C" << RESET << std::endl;
        cleanupResources();
        exit(0);
    }
}

void initializeCleanup(conf* conf_ptr) {
    g_conf = conf_ptr;
    signal(SIGINT, signalHandler);
}