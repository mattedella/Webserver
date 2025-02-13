SRC = main.cpp \
      Class/ABlock.cpp Class/Conf.cpp Class/Conf2.cpp Class/Http.cpp Class/Server.cpp Class/Server2.cpp Class/Request.cpp Class/Response.cpp \
	  Response/GenerateResp.cpp \
      Parsing/Parsing.cpp
CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98
RM = rm -f
NAME = webserv

FUCHSIA = '\033[0;95m'
CYAN = '\033[0;36m'
NONE = '\033[0m'
GREEN = '\033[0;32m'
RED = '\033[0;31m'
WHITE = '\033[0m'
A = '\033[0;34m'

all: $(NAME)

$(NAME): $(SRC)
	@echo $(A)			"Compiling webserv, please wait"$(NONE)
	@$(CC) $(CFLAGS) $(SRC) -o $(NAME)
	@clear
	@echo $(CYAN)		" __          __  _     _____                      __  "$(NONE) 
	@echo $(GREEN)		" \ \        / / | |   / ____|                  _  \ \ "$(NONE)
	@echo $(RED)		"  \ \  /\  / /__| |__| (___   ___ _ ____   __ (_)  | |"$(NONE)
	@echo $(FUCHSIA)	"   \ \/  \/ / _ \ '_ \\___  \ / _ \ '__\ \ / /      | |"$(NONE)
	@echo $(RED)		"    \  /\  /  __/ |_) |___) |  __/ |   \ V /   _   | |"$(NONE)
	@echo $(GREEN)		"     \/  \/ \___|_.__/_____/ \___|_|    \_/   (_)  | |"$(NONE)
	@echo $(CYAN)		"                                                  /_/ "$(NONE)

#--vgdb=yes --vgdb-error=0
val: all
	valgrind  --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes ./$(NAME) test.conf

run: all
	@./$(NAME) test.conf

clean:
	@clear

fclean: clean
	@$(RM) $(NAME)
	@echo $(RED)"Removed $(NAME)"$(WHITE)

re: fclean all

.PHONY: all clean fclean re