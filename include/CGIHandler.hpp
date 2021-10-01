#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <map>

#include "Config.hpp"
#include "Request.hpp"

class CGIHandler
{
  private:
  public:
    std::map<std::string, std::string> variables;
    CGIHandler(Config config, Request request);
    ~CGIHandler();
};

#endif