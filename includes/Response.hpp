#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Ablock.hpp"

class Response {

	private:
		std::string	_response;

	public:
		Response();

		std::string generateResponse(int client_socket);

		~Response();
};


#endif