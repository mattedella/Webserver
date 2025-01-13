#include "includes/webserv.hpp"

int StatusCode = 0;

int main (int argc, char **argv) {

	if (argc != 2) {
		std::cerr << "\033[31m" << "Error: Wrong number of argumets\n";
		std::cerr << "Use: " << argv[0] << " <configuration_file>\n" << "\033[0m";
		return 1;
	}
	std::ifstream config_file(argv[1]);
	if (!config_file.is_open()) {
		std::cerr << "\033[31m" <<"Error: Could not open the file " << argv[1] << "\033[0m"<< std::endl;
		return 1;
	}
	std::string newline;
	std::vector<std::string> conf_line;
	while (std::getline(config_file, newline)) {
		while (std::isspace(newline[0]))
			newline.erase(0, 1);
		conf_line.push_back(newline);
	}
	ParsConfFile(conf_line);
}
