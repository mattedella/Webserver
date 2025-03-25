#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Ablock.hpp"
#include "Conf.hpp"
#include "Request.hpp"
#include "Cgi.hpp"

class Response {

	private:
		std::string	_response;

		std::string sizeToString(size_t size);
		
	public:
		Response();

		void generateGetResponse(Request* req, conf* ConfBlock);
		void generatePostResponse(Request* req, conf* ConfBlock);
		void generateDeleteResponse(Request* req, conf* ConfBlock);
		void generateCGIResponse(Request* req, conf* ConfBlock);

		std::string generatePostBody();
		std::string getResponse();

		~Response();
};


#endif