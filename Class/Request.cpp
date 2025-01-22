
#include "../includes/webserv.hpp"
#include <cstdio>
#include <fstream>
#include <ios>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>
#include <poll.h>
#include <sys/types.h>
#include <dirent.h>

Request::Request() {}

std::string Request::getFileName() {
	return _nameFile;
}

std::string Request::getBody(const std::string& Key) {
	return _body[Key];
}

void Request::setPostName(std::string Path) {
	_POSTFile.open((Path + "/" + _nameFile).c_str(), std::ios::binary);
	if (!_POSTFile.is_open()) {
		std::cout << "Error: cannot open the file\n";
	}
	_POSTFile << _contentFile.data();
	_POSTFile.close();
}

void Request::clear() {
	_method.clear();
	_headers.clear();
	_file.clear();
	_httpVersion.clear();
	_url.clear();
	_body.clear();
}

void Request::parsGet(std::stringstream& file, std::string& line) {
	while (std::getline(file, line) && !line.empty()) {
			size_t colon_pos = line.find(':');
			if (colon_pos != NOT_FOUND) {
				std::string Key = line.substr(0, colon_pos);
				std::string Tp = line.substr(colon_pos + 2);
				_headers.insert(std::make_pair(Key, Tp));
			}
		}
}

void Request::parsDelete(std::stringstream& file, std::string& line) {
	(void)file;
	(void)line;
	std::cout << "parsDelete\n";
}

void Request::parsApplication(std::stringstream& bodyData, std::string& line) {
	std::string Key, Tp, value;
	while (std::getline(bodyData, line) && line.substr(0, line.length() - 1).empty())
		;
	std::getline(bodyData, line);
	if (line.find('&') != NOT_FOUND) {
		while (!line.empty()) {
			if (line.rfind('&') != NOT_FOUND) {
				value = line.substr(line.rfind('&') + 1);
				line.erase(line.rfind('&'), value.length() + 1);
			}
			else {
			 	value = line;
				line.erase(0, value.length());
			} 
			Key = value.substr(0, value.find('='));
			Tp = value.substr(value.find('=') + 1);
			_body.insert(std::make_pair(Key, Tp)); 
		}
	}
	else {
		Key = line.substr(0, line.find('='));
		Tp = line.substr(line.find('=') + 1);
		_body.insert(std::make_pair(Key, Tp));
	}
}

void Request::parsMultipart(std::stringstream& bodyData, std::string& line, std::string Type) {
    std::string Key, Tp, Value, Boundary, endBoundary;

    // Calcolo del Boundary
	std::cout<< "INIZIO PARSMULTYPART\n";
    std::cout << "Type: " << Type << std::endl;
    size_t boundaryPos = Type.find("boundary=");
    if (boundaryPos != std::string::npos) {
        Boundary = "--" + Type.substr(boundaryPos + 9); // 9 è la lunghezza di "boundary="
        endBoundary = Boundary + "--";
        std::cout << "Boundary calcolato: " << Boundary << std::endl;
        std::cout << "EndBoundary calcolato: " << endBoundary << std::endl;
    } else {
        std::cerr << "Errore: Boundary non trovato in Content-Type." << std::endl;
        return;
    }

    // Lettura del corpo della richiesta
    while (std::getline(bodyData, line)) {
        std::cout << "Riga letta: [" << line << "]" << std::endl;

        if (line.empty()) {
            std::cout << "Riga vuota, salto al prossimo ciclo." << std::endl;
            continue;
        }

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            Key = line.substr(0, colonPos);
            Tp = line.substr(colonPos + 2); // Salta ": " (due punti e spazio)
            std::cout << "Key trovata: [" << Key << "]" << std::endl;
            std::cout << "Tp trovata: [" << Tp << "]" << std::endl;

            _body.insert(std::map<std::string, std::string>::value_type(Key, Tp));
            std::cout << "Inserito in _body: {" << Key << ": " << Tp << "}" << std::endl;
        } else {
            std::cout << "Nessun ':' trovato nella riga, continuazione del ciclo." << std::endl;
        }
    }

    // Stato finale
    std::cout << "Parsing completato. Stato di _body:" << std::endl;
    std::map<std::string, std::string>::iterator it;
    for (it = _body.begin(); it != _body.end(); ++it) {
        std::cout << "  {" << it->first << ": " << it->second << "}" << std::endl;
    }
}



void Request::parsPost(std::stringstream& file, std::string& line) {
	std::string body, value, Key, Tp;
	while (std::getline(file, line) && !line.substr(0, line.length() - 1).empty()) {
		size_t colon_pos = line.find(':');
		if (colon_pos != NOT_FOUND) {
			std::string Key = line.substr(0, colon_pos);
			std::string Tp = line.substr(colon_pos + 2);
			_headers.insert(std::make_pair(Key, Tp));
		}
	}
	if (_headers["Content-Type"].find("application/x-www-form-urlencoded") == 0) 
	{
		parsApplication(file, line);
	}
	else if (_headers["Content-Type"].find("multipart/form-data") == 0) 
	{
		parsMultipart(file, line, _headers["Content-Type"]);
	}
	// else if (_headers["Content-Type"] == "application/json") // 
	// 	parsJSon(bodyData, line);
	// else if (_headers["Content-Type"] == "application/xml")
	// 	parsXml(bodyData, line);
	// else if (_headers["Content-Type"] == "text/plain")
	// 	parsText(bodyData, line);
	// else if (_headers["Content-Type"] == "application/octet-stream")
	// 	parsOctet(bodyData, line);
	// else if (_headers["Content-Type"] == "application/ld+json")
	// 	parsLdJson(bodyData, line);
	// else if (_headers["Content-Type"] == "text/csv")
	// 	parsTextCsv(bodyData, line);
	// else if (_headers["Content-Type"] == "application/graphql")
	// 	parsGraph(bodyData, line);
	// else if (_headers["Content-Type"] == "application/protobuf")
	// 	parsProtobuf(bodyData, line);
	// else if (_headers["Content-Type"] == "text/event-stream")
	// 	parsEventStream(bodyData, line);
	// else if (_headers["Content-Type"] == "application/zip")
	// 	parsZip(bodyData, line);
}

void Request::ParsRequest(std::stringstream& to_pars) {
	std::string line;
	std::getline(to_pars, line);
	std::stringstream req_line(line);
	req_line >> _method >> _url >> _httpVersion;
	if (_method == "GET")
		parsGet(to_pars, line);
	else if (_method == "POST")
		parsPost(to_pars, line);
	else if (_method == "DELETE")
		parsDelete(to_pars, line);
	}

void Request::getRequest(int &client_socket, short& event, int MaxSize) {
	int bytes_received = 0;
	std::stringstream buffer;
	char* tmp_buffer = new char[MaxSize];

	bytes_received = recv(client_socket, tmp_buffer, MaxSize, 0);
	if (bytes_received < 0) {
		throw exc("Error reading request\n");
	}
	buffer.write(tmp_buffer, bytes_received);
	delete [] tmp_buffer;
	ParsRequest(buffer);
	// printRequest();
	event = POLLOUT;
}


void Request::printRequest() {
	std::cout << "Method: " + _method + '\n';
	std::cout << "URL: " + _url + '\n';
	std::cout << "HTTP Version: " + _httpVersion + '\n';
	std::cout << "Headers: \n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first + ": " + it->second + '\n';
	std::cout << "Body: \n";
	for (std::map<std::string, std::string>::iterator bit = _body.begin(); bit != _body.end(); bit++)
		std::cout << bit->first + ": " + bit->second + '\n';
}

std::string Request::getHeader(const std::string& Key) {
	return _headers[Key];
}

std::string Request::getHttpVersion() {
	return _httpVersion;
}

std::string Request::getMethod() {
	return _method;
}

std::string& Request::getURL() {
	return _url;
}

Request::~Request() {}