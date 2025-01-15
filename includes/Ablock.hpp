
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

# define  NOT_FOUND std::string::npos

class ABlock {

	protected:
		std::multimap<std::string, std::string> _data;
		std::vector<std::string>				_special;
		std::map<int,std::string>				_error;
		std::string								_root;
		int 									_timeout;
		int 									_bodysize;
		std::vector<std::string>				_methods;
	
	public:
		ABlock();

		void initMap(std::string& str);
		size_t getSize() const;
		bool checkKey(std::string& Key);

		std::string	getErrorPage(int error);
		std::string	getRoot();
		int			getTimeout();
		int			getBodysize();
		bool		getMethods(std::string to_find);
		int			getMethodsSize();

		int		ErrorPageSize();
		bool	someInfo(std::string &str);
		void printMap();

		virtual ~ABlock();
};

#endif