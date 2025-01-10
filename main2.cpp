#include "includes/HttpServer.hpp"

// main.cpp
#include "includes/webserv.hpp"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file.conf>" << std::endl;
        return 1;
    }

    try {
        std::ifstream config_file(argv[1]);
        if (!config_file.is_open()) {
            std::cerr << "Error: Cannot open config file" << std::endl;
            return 1;
        }

        std::string line;
        std::vector<std::string> config_content;
        while (std::getline(config_file, line)) {
            while (std::isspace(line[0]))
                line.erase(0, 1);
            config_content.push_back(line);
        }

        server webServer;
        
        if (!webServer.init()) {
            std::cerr << "Error: Failed to initialize server" << std::endl;
            return 1;
        }

        ParsConfFile(config_content);
        
        webServer.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}