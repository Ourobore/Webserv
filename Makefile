SRCS_DIR	= src

SRCS_SERVER	= mainServer.cpp \
			  $(SRCS_DIR)/Server.cpp \
			  $(SRCS_DIR)/Socket.cpp

SRCS_CLIENT	= mainClient.cpp \
			  $(SRCS_DIR)/Client.cpp \
			  $(SRCS_DIR)/Socket.cpp

OBJS_SERVER	= $(addprefix $(OBJS_DIR), $(SRCS_SERVER:.cpp=.o))

OBJS_CLIENT	= $(addprefix $(OBJS_DIR), $(SRCS_CLIENT:.cpp=.o))

INCLUDE		= -Iinclude/

OBJS_DIR_SERVER	= objs_server/

OBJS_DIR_CLIENT	= objs_client/

CC			= clang++

CPPFLAGS	= -Wall -Werror -Wextra -std=c++98

SERVER_NAME	= server

CLIENT_NAME	= client

all			: $(SERVER_NAME) $(CLIENT_NAME)

$(OBJS_DIR)%.o	: ./%.cpp
			  	  $(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

$(SERVER_NAME)	: $(OBJS_DIR_SERVER) $(OBJS_SERVER)
			  $(CC) $(CPPFLAGS) $(OBJS_SERVER) -o $(SERVER_NAME)

$(CLIENT_NAME)	: $(OBJS_DIR_CLIENT) $(OBJS_CLIENT)
			  $(CC) $(CPPFLAGS) $(OBJS_CLIENT) -o $(CLIENT_NAME)

$(OBJS_DIR_SERVER)	:
			  @mkdir $(OBJS_DIR_SERVER)

$(OBJS_DIR_CLIENT)	:
			  @mkdir $(OBJS_DIR_CLIENT)

clean		:
		 	 rm -rf $(OBJS_SERVER) $(OBJS_CLIENT) $(OBJS_DIR_SERVER) $(OBJS_DIR_CLIENT) mainServer.o mainClient.o

fclean		: clean
		 	 rm -rf $(SERVER_NAME) $(CLIENT_NAME)

re			: clean all

.PHONY		: all clean re
