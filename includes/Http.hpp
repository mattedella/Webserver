
#ifndef HTTP_HPP
# define HTTP_HPP

#include "Ablock.hpp"
#include <vector>

class ABlock;



class http : public ABlock {

	private:
		std::vector<std::string> _include;


	public:
		http();

		void checkVal();
		void initVector();
		void printAll();
		void addVal();
		void printErrorPage();
		std::string getInclude(std::string to_find);

		~http();
};

#endif