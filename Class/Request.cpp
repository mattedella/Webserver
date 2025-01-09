
#include "../includes/Request.hpp"

Request::Request(): _url(NULL) {}

Request::Request(std::string& url): _url(url) {}

Request::~Request() {}