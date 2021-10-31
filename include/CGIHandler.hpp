#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "Config.hpp"
#include "Request.hpp"
#include "Socket.hpp"
#include "Webserv.hpp"
#include "utilities.hpp"

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

    void launch_cgi(ClientHandler& client, std::vector<struct pollfd>& pfds,
                    Config& server_config);

    static bool is_cgi_file(std::string filename, int location_index,
                            Config& server_config);
};

#endif