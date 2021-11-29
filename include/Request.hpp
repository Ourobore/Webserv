#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Chunk.hpp"
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
    std::vector<std::string> _index_names;
    int                      _location_index;

    Chunk* _chunk;

    void split_lines(std::string& req_str, std::vector<std::string>& req_lines);
    std::vector<std::string> split_tokens(std::string line,
                                          std::string delimiters);
    int  parse_first_header(std::vector<std::string>& req_lines,
                            Config&                   server_config);
    void parse_uri(Config& server_config);
    void parse_headers(std::vector<std::string>& req_lines);
    void parse_body(std::vector<std::string>& req_lines);
    void resolve_index();

  public:
    Request(std::string& bytes, Config& server_config);
    std::map<std::string, std::string> tokens;
    bool                               all_chunks_received;

    // Access request like a std::map
    std::string operator[](const std::string& key) const;

    // Accessors
    int                       location_index() const;
    std::vector<std::string>& index_names();
    Chunk*                    chunk();
    void                      set_chunk(Chunk* chunk);
};

#endif
