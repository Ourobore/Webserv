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

    void                     split_lines();
    std::vector<std::string> split_tokens(std::string line);
    int                      parse_first_header();
    void                     parse_headers();
    void                     parse_body();

  public:
    Request(char* bytes);
    std::map<std::string, std::string> tokens;
    std::string                        uri;
    std::string                        method;
};

#endif
