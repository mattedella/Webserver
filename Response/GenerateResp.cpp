
#include "../includes/webserv.hpp"
#include <sys/socket.h>

void sendResponse(int client_socket, conf ConfBlock, Request* req) {
	Response* res = new Response();
	// controllare se request giusta
	ConfBlock.checkRequest(req);
	if (req->getMethod() == "GET")
		res->generateGetResponse(req, ConfBlock);
	else if (req->getMethod() == "POST")
		std::cout << "POST Response\n";
		// res->generatePostResponse(req, ConfBlock, 2);
	else if (req->getMethod() == "DELETE")
		std::cout << "DELETE Response\n";
		// res->generateDeleteResponse(req, ConfBlock, 3);
	send(client_socket, res->getResponse().c_str(), res->getResponse().length(), 0);
}