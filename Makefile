################################################################################
# Webserv
SERVER		= server

# Compilation parameters
CXX			= clang++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -g3
INC			= -Iinclude/

# Source files
SRCDIR		=	src/
MAIN 		=	main.cpp
SRC			=	Socket.cpp \
				Server.cpp \
				Request.cpp \
				Webserv_connection.cpp \
				Webserv_request.cpp \
				Config.cpp \
				parsing_config.cpp \
				CGIHandler.cpp \
				FileHandler.cpp \
				utilities.cpp \

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