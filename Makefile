# CFLAGS = -Wall -Wextra -Werror -std=c++98
# SRCS = main.cpp \
# 	   WebServ.cpp \
# 	   Config.cpp \
# 	   Server.cpp \
# 	   Location.cpp \
# 	   Request.cpp \
# 	   RequestMaker.cpp \
# 	   Document.cpp \
# 	   Response.cpp \
# 	   RequestProcessor.cpp \
# 	   ResponseSender.cpp
# OBJS = $(SRCS:.cpp=.o)
# CC = c++
# TARGET = webserv

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# %.o: %.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS)

# fclean: clean
# 	rm -f $(TARGET)

# re: fclean all

# .PHONY: all clean fclean re


CFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = main.cpp \
	   WebServ.cpp \
	   Config.cpp \
	   Server.cpp \
	   Location.cpp \
	   Request.cpp \
	   RequestMaker.cpp \
	   Document.cpp \
	   Response.cpp \
	   RequestProcessor.cpp \
	   ResponseSender.cpp
OBJS = $(SRCS:.cpp=.o)
OBJS := $(addprefix obj/, $(OBJS)) # 오브젝트 파일 경로 수정
CC = c++
TARGET = webserv
OBJDIR = obj/

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJDIR)%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
