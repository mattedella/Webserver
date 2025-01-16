
#include "../includes/webserv.hpp"
#include <sys/socket.h>

void sendResponse(int client_socket, conf ConfBlock, Request* req, short& event) {
	Response* res = new Response();
	// controllare se request giusta
	if (req->getMethod() == "GET") {
		ConfBlock.checkGetRequest(req);
		res->generateGetResponse(req, ConfBlock);
	}
	else if (req->getMethod() == "POST")
		std::cout << "POST Response\n";
		// res->generatePostResponse(req, ConfBlock);
	else if (req->getMethod() == "DELETE")
		std::cout << "DELETE Response\n";
		// res->generateDeleteResponse(req, ConfBlock);
	int byte_send = send(client_socket, res->getResponse().c_str(), res->getResponse().length(), 0);
	if (byte_send <= 0) {
		delete res;
		throw exc("Error: response not send\n");
	}
	event = 0;
}