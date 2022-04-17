#SRC=./config/ConfigParser.cpp  ./config/Token.cpp \
# ./exceptions/IllegalArgumentException.cpp ./exceptions/FileNotFoundException.cpp  tools/Utils.cpp
SRC = $(shell find ./config -name "*.cpp") $(shell find ./network -name "*.cpp" ) $(shell find ./exceptions -name "*.cpp") $(shell find ./tools -name "*.cpp")
NAME=webserv

all: $(NAME)

$(NAME): $(SRC)
	c++ -g -o $(NAME) $(SRC)
clean:
	rm -f *.o
fclean: clean
	rm -f $(NAME)
re: fclean all
