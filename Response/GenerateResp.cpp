
#include "../includes/webserv.hpp"
#include <sys/socket.h>

void sendResponse(int client_socket, conf ConfBlock, Request* req) {
	Response* res = new Response();
	// controllare se request giusta
	std::cout << "genera risposta\n";
	std::cout << req->getURL();
	ConfBlock.checkRequest(req);
	res->generateResponse(req, ConfBlock);
	std::cout << "Response: |" << res->getResponse() << "|\n";
	send(client_socket, res->getResponse().c_str(), res->getResponse().length(), 0);
}