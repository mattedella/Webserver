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
		std::string							_contentFile;
		size_t								_contentLength;
		std::ofstream						_POSTFile;

	public:
		Request();

		void			ParsRequest(std::stringstream& to_pars, conf* ConfBlock);
		void			parsPost(std::stringstream& file, std::string& line, std::string Path);
		void			parsDelete(std::stringstream& file, std::string& line);
		void			parsApplication(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsMultipart(std::stringstream& bodyData, std::string& line, std::string Path, std::string Type);
		void			parsJSon(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsXml(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsText(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsOctet(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsLdJson(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsTextCsv(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsGraph(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsProtobuf(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsEventStream(std::stringstream& bodyData, std::string& line, std::string Path);
		void			parsZip(std::stringstream& bodyData, std::string& line, std::string Path);

		void			getRequest(int& client_socket, short& event, int MaxSize, conf* ConfBlock);
		std::string&	getURL();
		std::string		getHttpVersion();
		std::string		getMethod();
		std::string		getHeader(const std::string& Key);
		std::string		getBody(const std::string& Key);
		void			setPostName(std::string Path);
		std::string		getFileName();
		std::string		getContentFile();
		void			clear();

		void			printRequest();

		~Request();
};

#endif