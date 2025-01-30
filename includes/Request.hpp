#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class conf;

class Request {

	private:
		std::string							_url;
		std::string							_file;
		std::string							_httpVersion;
		std::string							_method;
		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_body;
		std::string							_nameFile;
		std::ofstream						_PostFile;
		
        bool                                _headers_complete;  // New
        bool                                _body_complete;     // New
        size_t                              _content_length;    // New

	public:
		Request();

		void			ParsRequest(std::stringstream& to_pars, conf* ConfBlock);
		void			parsPost(std::stringstream& file, std::string& line, std::string Path);
		void			parsDelete(std::stringstream& file, std::string& line);
		void			parsApplication(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsMultipart(std::stringstream& bodyData, std::string& Path, std::string Type);

		std::string&	getURL();
		std::string		getMethod();
		std::string		getFileName();
		std::ofstream&	getPostFile();
		std::string		getContentFile();
		std::string		getHttpVersion();
		std::string		getBody(const std::string& Key);
		std::string		getHeader(const std::string& Key);

		void			setPostName(std::string Path);
		void			setContentType(const std::string& fullPath);
		void			setPostContentType(const std::string& fullPath);
		void			setHeader(const std::string& Key, const std::string& Tp);
		std::string		generateBody();

		void			clear();
		void			closeFile();
		void			printRequest();
		void			getRequest(int& client_socket, short& event, int MaxSize, conf* ConfBlock);

		void			setHeadersComplete(bool complete);
		void			setBodyComplete(bool complete);
		void			setContentLength(size_t length);
		~Request();
};

#endif