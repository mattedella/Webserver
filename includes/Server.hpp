#ifndef SERVER_HPP
# define SERVER_HPP

#include "Ablock.hpp"
#include "Request.hpp"
#include <cstddef>
#include <string>
#include <sys/poll.h>
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
		struct pollfd*					_poll_fds;
		size_t							_pollfd_size;
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
		void	checkPort(server to_check);
		bool	checkLocation(std::string to_find);
				void	starting();
		
		
		bool	init(int port, int i);
		void	s_run(conf* ConfBlock, Request* req, int ret);
		void	handle_new_connection(int server_fd);
		void	handle_client_data(int index);
		void	handle_client_response(int index);
		void	handle_request(int client_fd, const std::string& request);
		void	close_connection(size_t index);
		void	closeSocket();
		void	set_nonblocking(int fd);
		void	addNametoHost();
		void	removeNameToHost();

    	int getPort() const;

		bool		getListing();
		std::string	getIndex();
		void 		startListens();
		location	getLocation(std::string to_find);
		bool getListen(std::string& to_find);
		bool getServerName(std::string& to_find);
		std::vector<std::string>& getServerNames() {
        	return _server_names;
		}

		~server();
};

#endif