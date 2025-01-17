#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <sstream>
#include <string>

class Request {

	private:
		std::string							_url;
		std::string							_file;
		std::string							_httpVersion;
		std::string							_method;
		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_body;

	public:
		Request();

		void			ParsRequest(std::string& to_pars);
		void			parsGet(std::stringstream& file, std::string& line);
		void			parsPost(std::stringstream& file, std::string& line);
		void			parsDelete(std::stringstream& file, std::string& line);

		void			getRequest(int& client_socket, short& event, int MaxSize);
		std::string&	getURL();
		std::string		getHttpVersion();
		std::string		getMethod();
		std::string		getHeader(const std::string& Key);
		void			clear();

		void			printRequest();

		~Request();
};

#endif