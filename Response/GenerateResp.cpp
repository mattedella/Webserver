
#include "../includes/webserv.hpp"
#include <sys/socket.h>

bool sendResponse(int client_socket, conf* ConfBlock, Request* req, short& event) {
	Response* res = new Response();
	(void)event;
	if (req->getMethod() == "GET")
		res->generateGetResponse(req, ConfBlock);
	else if (req->getMethod() == "POST")
		res->generatePostResponse(req, ConfBlock);
	else if (req->getMethod() == "DELETE")
		res->generateDeleteResponse(req, ConfBlock);
	int byte_send = send(client_socket, res->getResponse().c_str(), res->getResponse().length(), 0);
	bool ret = true;
	if (byte_send < 0) {
		std::cout << "Error: response not send\n";
		ret = false;
	}
	if (res != NULL)
		delete res;
	return ret;
}