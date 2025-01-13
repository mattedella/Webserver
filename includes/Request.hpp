#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <string>

// TCP connection, then GET or "methods" request
// after response, TCP conection is closed

class Request {

	private:
		std::string							_url;
		std::string							_httpVersion;
		std::string							_method;
		std::map<std::string, std::string>	_headers;

	public:
		Request();

		void		ParsRequest(char* Request);
		std::string	getURL();
		std::string	getHttpVersion();
		std::string	getMethod();
		std::string	getHeader(const std::string& Key);
		void		printRequest();

		~Request();
};

#endif