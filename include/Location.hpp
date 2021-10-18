#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

class Location
{
  private:
    Location();

    std::string                         path;
    std::map<std::string, bool>         methods;
    std::string                         root;
    std::string                         autoindex;
    std::vector<std::string>            index;
    std::pair<std::string, std::string> redir;
    std::string                         upload;
    std::string                         cgi_pass;
    std::string                         cgi_extension;

    void set_method(std::string line);
    void set_root(std::string line);
    void set_autoindex(std::string line);
    void set_index(std::string line);
    void set_redir(std::string line);
    void set_upload(std::string line);
    void set_cgi_pass(std::string line);
    void set_cgi_extension(std::string line);

  public:
    Location(std::string block);
    ~Location();

    std::string search_config(std::string config, std::string key);

    std::string                         get_path() const;
    std::map<std::string, bool>         get_methods() const;
    std::string                         get_root() const;
    std::string                         get_autoindex() const;
    std::vector<std::string>            get_index() const;
    std::pair<std::string, std::string> get_redir() const;
    std::string                         get_upload() const;
    std::string                         get_cgi_pass() const;
    std::string                         get_cgi_extension() const;
};

std::ostream& operator<<(std::ostream& os, Location const& src);

#endif