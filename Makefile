SRC = main.cpp \
      Class/ABlock.cpp Class/Conf.cpp Class/Http.cpp Class/Server.cpp Class/Server2.cpp\
      Parsing/Parsing.cpp Class/Conf2.cpp Class/Response.cpp Class/Request.cpp Response/GenerateResp.cpp\
CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98
RM = rm -f
NAME = webserv
GREEN = \033[0;32m
RED = \033[0;31m
WHITE = \033[0m

all: $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CFLAGS) $(SRC) -o $(NAME)
	@clear
	@echo "$(GREEN)Compiled $(NAME)$(WHITE)"

clean:
	@clear
	@echo "$(RED)Nothing to clean, no .o files generated.$(WHITE)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Removed $(NAME)$(WHITE)"

re: fclean all

.PHONY: all clean fclean re