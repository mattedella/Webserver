#include "includes/webserv.hpp"
#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>

int main(int ac, char **av) {
	
	if (ac != 2)
		return (std::cerr<<"Error: wrong number of arguments\n", 1);
	
	std::ifstream myfile;
	myfile.open(av[1]);
	if (!myfile.is_open())
		return (std::cerr << "Error: file corrupted or inexistent\n", 1);
	ParsFile(myfile);
	myfile.close();
	return 0;
}