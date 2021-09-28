#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <algorithm>
#include <iostream>
#include <istream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Request
{
  private:
    Request();
    std::string              req_str;
    std::vector<std::string> req_lines;

    void split_lines();

  public:
    Request(char* bytes);
    std::map<std::string, std::string> request;
    std::string                        method;
    std::string                        uri;
};

#endif
