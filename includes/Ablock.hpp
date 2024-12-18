
#ifndef ABLOCK_HPP
# define ABLOCK_HPP

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <map>
#include <vector>
#include <exception>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>
#include <cctype>
#include <cstdio>
#include <exception>
#include <sstream>

class ABlock {

	protected:
		std::multimap<std::string, std::string> _data;
		std::vector<std::string>				_special;
	
	public:
		ABlock();
		void initMap(std::string& str);
		size_t getSize() const;
		bool checkKey(std::string& Key);

		bool someInfo(std::string &str);
		void printMap();
		virtual ~ABlock();
};

#endif