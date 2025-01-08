
#ifndef EXC_HPP
# define EXC_HPP

#include "Ablock.hpp"

class exc: public std::exception
{
	private:
		std::string _err;
	public:
		exc(const std::string &err): _err(err) { };
		~exc() throw() {};
		const char* what() const throw()
		{
			
			return _err.c_str();
		};
};

#endif