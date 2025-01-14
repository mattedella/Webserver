
#include "../includes/webserv.hpp"
#include <sys/socket.h>

Request* getRequest(int client_socket) {
	Request* ret = new Request();
	int bytes_recived;
	char buffer[4096];

	bytes_recived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_recived < 0) {
		delete(ret);
		throw exc("Error reading request\n");
	}
	buffer[bytes_recived] = '\0';
	ret->ParsRequest(buffer);
	return ret;
}

void sendResponse(int client_socket, conf ConfBlock) {
	Request* req = getRequest(client_socket);
	Response* res = new Response();
	// controllare se request giusta
	ConfBlock.checkRequest(req);
	res->generateResponse(req, ConfBlock);
	send(client_socket, res->getResponse().c_str(), res->getResponse().length(), 0);
}