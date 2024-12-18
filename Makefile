SRC =	main.cpp \
		Class/ABlock.cpp Class/Conf.cpp Class/Http.cpp Class/Server.cpp \
		Parsing/Parsing.cpp
OBJ = $(SRC:.cpp=.o)
CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98
RM = rm -f
NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
		@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
		@echo "Compiled $(NAME)"

%.o: %.cpp
		@$(CC) $(CFLAGS) -c $< -o $@

clean:
		@$(RM) $(OBJ)
		@echo "Removed object files"

fclean: clean
		@$(RM) $(NAME)
		@echo "Removed $(NAME)"

re: fclean all

.PHONY: all clean fclean re