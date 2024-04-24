CFLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++
TARGET = webserv
OBJDIR = obj
SRCDIR = .

# 소스 파일 정의
SRCS = main.cpp \
	   Webserv.cpp \
	   Config/Config.cpp \
	   Config/Server.cpp \
	   Config/Location.cpp \
	   Request.cpp \
	   Doc.cpp \
	   Response.cpp 

# 오브젝트 파일 경로 변환
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

# 대상 파일과 오브젝트 파일 간의 종속성 정의
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# 오브젝트 파일 컴파일 규칙
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

# 청소 규칙
clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(TARGET)

re:
	make fclean
	make all

.PHONY: all clean fclean re

