#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <poll.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "ClientHandler.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Socket.hpp"
#include "utilities.hpp"

#define PIPEREAD 0
#define PIPEWRITE 1
#define STDIN 0
#define STDOUT 1

class CGIHandler
{
  private:
    std::map<std::string, std::string> variables;
    char**                             env_array;
    char**                             cgi_argv;

    std::string cgi_path;
    std::string script_name;
    std::string root_directory;

    char** get_env_array();

    void        DEBUG_print_env_array() const;
    std::string getOsName();

  public:
    CGIHandler(Config& config, Request& request, int client_fd);
    ~CGIHandler();

    void setup_cgi(ClientHandler& client, std::vector<struct pollfd>& pfds,
                   Config& server_config);
    void launch_cgi();

    static bool is_cgi_file(std::string filename, int location_index,
                            Config& server_config);

    int* input_pipe;
    int* output_pipe;
};

#endif