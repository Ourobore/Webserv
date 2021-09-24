#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

class Request
{
  private:
    Request();
    std::string reqstr;

    std::vector<std::string> split_tokens();

  public:
    Request(char* bytes);
    std::string method;
    std::string url;
};

#endif