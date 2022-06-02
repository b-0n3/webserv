
SRC = ./src/config/Page.cpp \
      ./src/config/Redirect.cpp \
      ./src/config/Location.cpp \
      ./src/config/ConfigParser.cpp \
      ./src/config/Cgi.cpp \
      ./src/config/Token.cpp \
	  ./src/network/HttpServlet.cpp \
      ./src/network/HttpResponse.cpp \
      ./src/network/HttpRequest.cpp \
      ./src/config/Server.cpp \
      ./src/exceptions/FileNotFoundException.cpp \
      ./src/exceptions/IllegalStateException.cpp \
      ./src/exceptions/IllegalArgumentException.cpp \
      ./src/tools/Utils.cpp \
      ./src/tools/TempFile.cpp \
      ./src/tools/Logger.cpp \
      ./src/network/Server.cpp


NAME=webserv

OBJ=$(SRC:.cpp=.o)

FLAGS=-Wall -Wextra -Werror


%.o: %.cpp
	c++ $(FLAGS) -o $@ -c  $<

all: $(NAME)


$(NAME): $(OBJ)
	c++ -Wall -Wextra -Werror  $(OBJ) -o $(NAME)
clean:
	rm  -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
