#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <string>

// TCP connection, then GET or "methods" request
// after response, TCP conection is closed

class Request {

	private:
		std::string _url;
		std::string	_ipAddres;
		std::string	_method;

	public:
		Request();
		Request(std::string& url);

		~Request();
};

#endif