#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <string>

// TCP connection, then GET or "methods" request
// after response, TCP conection is closed

class Request {

	private:
		std::string							_url;
		std::string							_file;
		std::string							_httpVersion;
		std::string							_method;
		std::map<std::string, std::string>	_headers;
		
        bool                                _headers_complete;  // New
        bool                                _body_complete;     // New
        size_t                              _content_length;    // New

	public:
		Request();

		void		ParsRequest(char* Request);
		void		getRequest(int& client_socket, short& event);
		std::string&	getURL();
		std::string	getHttpVersion();
		std::string	getMethod();
		std::string	getHeader(const std::string& Key);
		void		printRequest();

        bool        isComplete() const;
        void        setHeadersComplete(bool complete);
        void        setBodyComplete(bool complete);
        void        setContentLength(size_t length);
        void        clear();  

		~Request();
};

#endif