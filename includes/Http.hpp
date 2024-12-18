
#ifndef HTTP_HPP
# define HTTP_HPP

#include "Ablock.hpp"

class ABlock;

class http : public ABlock {

	private:
		std::map<int,std::string> _error;

	public:
		http();

		void initVector();
		void printErrorPage();

		~http();
};

#endif