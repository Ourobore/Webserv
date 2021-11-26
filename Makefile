################################################################################
# Webserv
SERVER		= server

# Compilation parameters
CXX			= clang++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -g -fstandalone-debug -fsanitize=address
INC			= -Iinclude/

# Source files
SRCDIR		=	src/
MAIN 		=	main.cpp
SRC			=	Webserv_connection.cpp \
				Webserv_request.cpp \
				Webserv_poll.cpp \
				socket/Socket.cpp \
				socket/Server.cpp \
				config/Config.cpp \
				config/Location.cpp \
				config/parsing_config.cpp \
				modules/CGIHandler.cpp \
				modules/FileHandler.cpp \
				modules/ClientHandler.cpp \
				utilities/utilities.cpp \
				utilities/generate.cpp \
				request/Request.cpp \
				request/parsing_mimetypes.cpp \
				methods/upload.cpp \
				methods/delete.cpp \
				methods/post.cpp \

# *.o files
OBJDIR = obj/
OBJ = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))
MAIN_OBJ = $(addprefix $(OBJDIR),$(MAIN:.cpp=.o))

################################################################################

all: $(SERVER)

# Create main executable `server`
$(SERVER): $(OBJDIR) $(OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $(INC) $(MAIN_OBJ) $(OBJ) -o $(SERVER)

# Create an obj/ directory with all the *.o files
$(OBJDIR):
	mkdir -p $@
	mkdir -p $@config
	mkdir -p $@methods
	mkdir -p $@modules
	mkdir -p $@request
	mkdir -p $@socket
	mkdir -p $@utilities

# Compile the *.o
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJDIR) vgcore*

fclean: clean
	rm -rf $(SERVER) a.out*

re: clean fclean all

.PHONY		: all clean re

################################################################################