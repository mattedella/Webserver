#ifndef SERVER_HPP
# define SERVER_HPP

#include "Ablock.hpp"
#include "Request.hpp"
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

static const int BUFFER_SIZE = 1024;

class ABlock;
class conf;

class location : public ABlock {


	private:
		std::string					_index;
		bool						_listing;

	public:
		location();

		void		addVal();
		void		printVal();
		bool		getListing();
		std::string	getIndex();
		
		~location();
};

class server : public ABlock {

	private:
		struct ServerSocket
		{
			int fd;
			int port;
		};
		std::map<int, std::string>		_client_buffers;
		std::vector<ServerSocket>		_server_sockets;
		std::vector<int>				_server_fd;
		std::vector<struct pollfd>		_poll_fds;
		std::map<int, std::string> 		_client_responses;

		std::string						_index;
		bool							_listing;
		std::vector<std::string>		_listens;
		std::vector<int>				_ports;
		std::vector<std::string>		_server_names;
		std::map<std::string, location>	_locations;
		

	public:
		server();
		void 	printFdsVect();
		int		stop();
		void	printAll();
		int		start();
		void	initVector();
		void	printLocation();
		size_t	getLocationSize() const;
		void	addLocation(const std::string& Key, location loc);
		void	addVal();
		void	checkValue();
		bool	checkLocation(std::string to_find);
				void	starting();
		
		
		bool	init(int port);
		void	s_run(conf* ConfBlock, Request* req);
		void	handle_new_connection(int server_fd);
		void	handle_client_data(int index);
		void	handle_client_response(int index);
		void	handle_request(int client_fd, const std::string& request);
		void	close_connection(int index);
		void	closeSocket();
		void	set_nonblocking(int fd);
		void	addNametoHost();

    	int getPort() const;

		bool		getListing();
		std::string	getIndex();
		void 		startListens();
		location	getLocation(std::string to_find);
		std::string getListen(std::string& to_find);
		std::string getServerName(std::string& to_find);

		~server();
};

#endif