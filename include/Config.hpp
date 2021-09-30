#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cctype>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

class Config
{
  private:
    Config();

    std::string              host;
    int                      port;
    std::vector<std::string> server_names;
    int                      client_max;
    std::string              root;
    std::vector<std::string> index;

    void set_host_port(std::string line);
    void set_server_name(std::string line);
    void set_client_max(std::string line);
    void set_root(std::string line);
    void set_index(std::string line);

  public:
    Config(std::string config);
    Config(Config const& src);
    ~Config();

    std::string search_config(std::string config, std::string key);

    std::string              get_host() const;
    int                      get_port() const;
    std::vector<std::string> get_server_names() const;
    int                      get_client_max() const;
    std::string              get_root() const;
    std::vector<std::string> get_index() const;
};

std::ostream& operator<<(std::ostream& os, Config const& src);

#endif