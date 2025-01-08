#ifndef SERVER_HPP
# define SERVER_HPP

#include "Ablock.hpp"
#include <string>

class ABlock;

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
		std::string						_index;
		bool							_listing;
		std::vector<int>				_sockets;
		std::vector<std::string>		_listens;
		std::vector<std::string>		_server_names;
		std::map<std::string, location>	_locations;

	public:
		server();
		bool init(int port) ;
		int		stop();
		void	printAll();
		int		start();
		int		accept();
		void	initVector();
		void	printLocation();
		size_t	getLocationSize() const;
		void	addLocation(const std::string& Key, location loc);
		void	addVal();
		void	checkValue();

		bool		getListing();
		std::string	getIndex();
		location	getLocation(std::string& to_find);
		std::string getListen(std::string& to_find);
		std::string getServerName(std::string& to_find);

		~server();
};

#endif