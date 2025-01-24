
#include "../includes/webserv.hpp"
#include <sys/socket.h>

void sendResponse(int client_socket, conf* ConfBlock, Request* req, short& event) {
	Response* res = new Response();
	// controllare se request giusta
	if (req->getMethod() == "GET")
		res->generateGetResponse(req, ConfBlock);
	else if (req->getMethod() == "POST")
		res->generatePostResponse(req, ConfBlock);
	else if (req->getMethod() == "DELETE")
		// res->generateDeleteResponse(req, ConfBlock);
	std::cout << res->getResponse() << std::endl;
	int byte_send = send(client_socket, res->getResponse().c_str(), res->getResponse().length(), 0);
	if (byte_send <= 0)
		std::cout << "Error: response not send\n";
	delete res;
	event = 0;
}