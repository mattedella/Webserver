#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Ablock.hpp"
#include <fstream>
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
		std::string							_nameFile;
		std::string							_contentFile;
		std::ofstream						_POSTFile;

	public:
		Request();

		void			ParsRequest(std::string& to_pars);
		void			parsGet(std::stringstream& file, std::string& line);
		void			parsPost(std::stringstream& file, std::string& line);
		void			parsDelete(std::stringstream& file, std::string& line);
		void			parsApplication(std::stringstream& file, std::string& line);
		void			parsMultipart(std::stringstream& file, std::string& line, std::string Type);
		void			parsJSon(std::stringstream& file, std::string& line);
		void			parsXml(std::stringstream& file, std::string& line);
		void			parsText(std::stringstream& file, std::string& line);
		void			parsOctet(std::stringstream& file, std::string& line);
		void			parsLdJson(std::stringstream& file, std::string& line);
		void			parsTextCsv(std::stringstream& file, std::string& line);
		void			parsGraph(std::stringstream& file, std::string& line);
		void			parsProtobuf(std::stringstream& file, std::string& line);
		void			parsEventStream(std::stringstream& file, std::string& line);
		void			parsZip(std::stringstream& file, std::string& line);

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