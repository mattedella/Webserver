#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Ablock.hpp"
#include "Conf.hpp"
#include "Request.hpp"

class Response {

	private:
		std::string	_response;

	public:
		Response();

		void generateResponse(Request* req, conf ConfBlock);

		~Response();
};


#endif