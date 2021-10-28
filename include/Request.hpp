#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Config.hpp"
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
    std::vector<std::string> _index_names;

    void                     split_lines();
    std::vector<std::string> split_tokens(std::string line);
    int                      parse_first_header(Config& server_config);
    void                     parse_uri(Config& server_config);
    void                     parse_headers();
    void                     parse_body();

  public:
    Request(const char* bytes, Config& server_config);
    std::map<std::string, std::string> tokens;

    std::string operator[](const std::string& key) const;

    std::vector<std::string>& index_names();
};

#endif
