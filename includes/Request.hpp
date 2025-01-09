#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <string>

# define  NOT_FOUND std::string::npos

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

		void	ParsRequest(std::string& Request);
		void	printRequest();

		~Request();
};

#endif