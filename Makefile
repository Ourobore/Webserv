################################################################################
# Webserv
SERVER		= server
CLIENT		= client

# Compilation parameters
CXX			= clang++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98
INC			= -Iinclude/

# Source files
SRCDIR		=	src/
MAIN 		=	main.cpp
CLIENT_MAIN =	mainClient.cpp
SRC			=	Socket.cpp \
				Server.cpp \
				Client.cpp \
				Request.cpp

# *.o files
OBJDIR = obj/
OBJ = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))
MAIN_OBJ = $(addprefix $(OBJDIR),$(MAIN:.cpp=.o))
CLIENT_MAIN_OBJ = $(addprefix $(OBJDIR),$(CLIENT_MAIN:.cpp=.o))

################################################################################

all: $(SERVER) $(CLIENT)

# Create main executable `server`
$(SERVER): $(OBJDIR) $(OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $(INC) $(MAIN_OBJ) $(OBJ) -o $(SERVER)

# Create the `client` executable
$(CLIENT): $(OBJDIR) $(OBJ) $(CLIENT_MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $(INC) $(CLIENT_MAIN_OBJ) $(OBJ) -o $(CLIENT)

# Create an obj/ directory with all the *.o files
$(OBJDIR):
	mkdir -p $@

# Compile the *.o
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJDIR) vgcore*

fclean: clean
	rm -rf $(SERVER) $(CLIENT) a.out*

re: clean fclean all

.PHONY		: all clean re

################################################################################