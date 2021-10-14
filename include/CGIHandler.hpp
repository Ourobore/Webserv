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

    char**      get_env_array();
    void        DEBUG_print_env_array() const;
    std::string getOsName();

  public:
    CGIHandler(Config const& config, Request const& request, int client_fd);
    ~CGIHandler();

    void execute(char buffer[30000]);
};

#endif