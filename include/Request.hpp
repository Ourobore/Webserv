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
    int                      _location_index;

    void                     split_lines();
    std::vector<std::string> split_tokens(std::string line);
    int                      parse_first_header(Config& server_config);
    void                     parse_uri(Config& server_config);
    void                     parse_headers();
    void                     parse_body();
    void                     resolve_index();

  public:
    Request(const char* bytes, Config& server_config);
    std::map<std::string, std::string> tokens;

    // Access request like a std::map
    std::string operator[](const std::string& key) const;

    // Accessors
    int                       location_index() const;
    std::vector<std::string>& index_names();
};

#endif
