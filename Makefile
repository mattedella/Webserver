SRC = main.cpp \
      Class/ABlock.cpp Class/Conf.cpp Class/Http.cpp Class/Server.cpp \
      Parsing/Parsing.cpp
CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98
RM = rm -f
NAME = webserv

all: $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CFLAGS) $(SRC) -o $(NAME)
	@echo "Compiled $(NAME)"

clean:
	@echo "Nothing to clean, no .o files generated."

fclean: clean
	@$(RM) $(NAME)
	@echo "Removed $(NAME)"

re: fclean all

.PHONY: all clean fclean re