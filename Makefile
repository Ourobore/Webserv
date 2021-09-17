SRCS_DIR	= src

#SRCS		= $(SRCS_DIR)/Server.cpp \
#			  $(SRCS_DIR)/Socket.cpp \
#			  $(SRCS_DIR)/ClientSide.cpp \
#			  $(SRCS_DIR)/Clients.cpp \
#			  $(SRCS_DIR)/Socket.cpp

SRCS_SERVER	= $(SRCS_DIR)/Server.cpp \
			  $(SRCS_DIR)/Clients.cpp \
			  $(SRCS_DIR)/Socket.cpp

SRCS_CLIENT	= $(SRCS_DIR)/ClientSide.cpp \
			  $(SRCS_DIR)/Socket.cpp

OBJS_SERVER	= $(SRCS_SERVER:.cpp=.o)

OBJS_CLIENT	= $(SRCS_CLIENT:.cpp=.o)

#OBJS	= $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))

INCLUDE		= -Iinclude/

#OBJS_DIR_SERVER	= objs_server/

#OBJS_DIR_CLIENT	= objs_client/

CC			= clang++

CPPFLAGS	= -Wall -Werror -Wextra -std=c++98

SERVER_NAME	= server

CLIENT_NAME	= client

all			:  $(CLIENT_NAME) $(SERVER_NAME)

$(OBJS_S)%.o	: ./%.cpp
			  	  $(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

$(OBJS_C)%.o	: ./%.cpp
			  	  $(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

$(SERVER_NAME)	: $(OBJS_SERVER) $(OBJS_S)
			  $(CC) $(CPPFLAGS) mainServer.cpp $(OBJS_SERVER) $(INCLUDE) -o $(SERVER_NAME)

$(CLIENT_NAME)	: $(OBJS_CLIENT) $(OBJS_C)
			  $(CC) $(CPPFLAGS) mainClient.cpp $(OBJS_CLIENT) $(INCLUDE) -o $(CLIENT_NAME)

#$(OBJS_DIR_SERVER)	:
#			  @mkdir $(OBJS_DIR_SERVER)

#$(OBJS_DIR_CLIENT)	:
#			  @mkdir $(OBJS_DIR_CLIENT)

clean		:
		 	 rm -rf $(OBJS_SERVER) $(OBJS_CLIENT) $(OBJS_DIR_SERVER) $(OBJS_DIR_CLIENT) mainServer.o mainClient.o

fclean		: clean
		 	 rm -rf $(SERVER_NAME) $(CLIENT_NAME)

re			: clean all

.PHONY		: all clean re
