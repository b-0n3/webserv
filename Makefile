SRC=$(shell find . -name "*.cpp")
NAME=webserv

all: $(NAME)

$(NAME): $(SRC)
	c++ -o $(NAME) $(SRC)
clean:
	rm -f *.o
fclean: clean
	rm -f $(NAME)
re: fclean all
