CFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = Webserv.cpp \
	   Config.cpp \
	   Server.cpp \
	   Location.cpp \
	   Request.cpp \
OBJS = $(SRCS:.cpp=.o)
CC = c++
TARGET = webserv

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
