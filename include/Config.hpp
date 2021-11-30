#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Location.hpp"
#include <cctype>
#include <cstddef>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

class Location;

class Config
{
  private:
    Config();

    std::string                        host;
    std::vector<int>                   port;
    std::vector<std::string>           server_names;
    std::map<std::string, std::string> error_pages;
    size_t                             client_max;
    std::string                        root;
    std::vector<std::string>           index;
    std::vector<Location>              location;
    std::map<std::string, std::string> _mimetypes;

    void set_host_port(std::string line);
    void set_server_name(std::string line);
    void set_error_page(std::string config);
    void set_client_max(std::string line);
    void set_root(std::string line);
    void set_index(std::string line);
    void set_location(std::string config);

  public:
    Config(std::string config);
    Config(Config const& src);
    ~Config();

    std::string         search_config(std::string config, std::string key);
    std::vector<size_t> search_location(std::string config);

    std::string                        get_host() const;
    std::vector<int>                   get_port() const;
    std::vector<std::string>           get_server_names() const;
    std::map<std::string, std::string> get_error_pages() const;
    size_t                             get_client_max() const;
    std::string                        get_root() const;
    std::vector<std::string>           get_index() const;
    std::vector<Location>              get_locations() const;

    // MIME types
    void set_mimetypes(std::map<std::string, std::string> mimetypes);
    std::map<std::string, std::string> get_mimetypes() const;

    // Panic button
    void clean_all();
};

std::ostream& operator<<(std::ostream& os, Config const& src);

#endif