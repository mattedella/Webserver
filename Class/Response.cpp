
#include "../includes/webserv.hpp"

void Response::generateResponse(Request* req, conf ConfBlock) {
	switch (StatusCode) {
		case 200:
			_response = "HTTP/1.1 200 OK\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n";
			break ;
		case 404:
			_response = "HTTP/1.1 404 Not Found\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n";
			break ;
		case 403:
			_response = "HTTP/1.1 403 Forbidden\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n";
			break ;
		case 408:
			_response = "HTTP/1.1 408 Request Timeout\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n";
			break ;
		case 500:
			_response = "HTTP/1.1 500 Internal Server Error\r\n Content-Type: "
					+ req->getHeader("Content-Type")
					+ "\r\nConnection: close\r\n";
			break ;
		default:
			delete req;
			throw exc("Unespected behavior\n");	
	}
}