#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <sys/wait.h>
#include <unistd.h>

#include "Config.hpp"
#include "Request.hpp"

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

    char** get_env_array();

  public:
    CGIHandler(Request request);
    ~CGIHandler();

    void execute(char buffer[30000]);
};

#endif