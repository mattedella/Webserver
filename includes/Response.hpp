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

		void generateGetResponse(Request* req, conf* ConfBlock);
		void generatePostResponse(Request* req, conf* ConfBlock);
		void generateDeleteResponse(Request* req, conf* ConfBlock);

		std::string getResponse();

		~Response();
};


#endif