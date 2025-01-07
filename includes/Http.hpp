
#ifndef HTTP_HPP
# define HTTP_HPP

#include "Ablock.hpp"
#include <vector>

class ABlock;



class http : public ABlock {

	private:
		std::map<int,std::string> _error;
		std::vector<int> _timeout;
		std::vector<int> _bodysize;
		std::vector<std::string> _include;


	public:
		http();

		void checkVal();
		void initVector();
		void printAll();
		void addVal();
		void printErrorPage();

		~http();
};

#endif