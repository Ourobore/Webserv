#include "Config.hpp"
#include <iostream>
#include <string>

Config::Config(std::string config)
{
    set_host_port(search_config(config, "listen"));
    set_server_name(search_config(config, "server_name"));
    set_error_page(config);
    set_client_max(search_config(config, "client_max_body_size"));
    set_root(search_config(config, "root"));
    set_index(search_config(config, "index"));
}

Config::Config(Config const& src)
{
    *this = src;
}

Config::~Config()
{
}

std::string Config::search_config(std::string config, std::string key)
{
    size_t      begin;
    size_t      end;
    std::string line;

    begin = config.find(key);
    if (begin == std::string::npos)
        return ("");
    if (config[begin + key.size()] != ' ')
        throw std::string("Error: Missing space for " + key);
    end = config.find(";", begin);
    if (config.find(key, end + 1) != std::string::npos)
        throw std::string("Error: Double " + key);
    line = config.substr(begin, (end - begin));

    return (line);
}

void Config::set_host_port(std::string line)
{
    size_t            space_pos;
    size_t            split_pos;
    std::stringstream ss;

    if (line == "")
    {
        this->host = "0.0.0.0";
        this->port = 80;
        return;
    }
    space_pos = line.find(" ");
    split_pos = line.find(":");
    if (split_pos == std::string::npos)
    {
        for (size_t i = space_pos + 1; i < line.size(); i++)
            if (!std::isdigit(line[i]))
            {
                this->host = line.substr(space_pos + 1);
                this->port = 80;
            }
        if (this->port != 80)
        {
            this->host = "0.0.0.0";
            ss << line.substr(space_pos + 1);
            ss >> this->port;
        }
    }
    else
    {
        this->host = line.substr(space_pos + 1, (split_pos - space_pos - 1));
        ss << line.substr(split_pos + 1);
        ss >> this->port;
    }
}

void Config::set_server_name(std::string line)
{
    size_t space_pos;

    if (line == "")
        return;
    space_pos = line.find(" ");
    while (space_pos != std::string::npos)
    {
        if (line.find(" ", space_pos + 1) != std::string::npos)
            this->server_names.push_back(line.substr(
                space_pos + 1, line.find(' ', space_pos + 1) - space_pos - 1));
        else
            this->server_names.push_back(line.substr(space_pos + 1));
        space_pos = line.find(' ', space_pos + 1);
    }
}

void Config::set_error_page(std::string config)
{
    size_t      begin;
    size_t      end;
    size_t      begin_code;
    size_t      end_code;
    std::string line;
    std::string code;
    std::string value;

    begin = 0;
    while ((begin = config.find("error_page", begin + 1)) != std::string::npos)
    {
        if (config[begin + 10] != ' ')
            throw std::string("Error: Missing space for error_page");
        end = config.find(";", begin);
        line = config.substr(begin, (end - begin));
        value = line.substr(line.find_last_of(" ") + 1);
        begin_code = line.find(" ") + 1;
        end_code = begin_code;
        while ((end_code = line.find(" ", end_code + 1)) != std::string::npos)
        {
            code = line.substr(begin_code, end_code - begin_code);
            this->error_pages[code] = value;
            begin_code = end_code + 1;
        }
        begin = end;
    }
    return;
}

void Config::set_client_max(std::string line)
{
    std::stringstream ss;

    if (line == "")
        this->client_max = 10;
    else
    {
        ss << line.substr(line.find(" ") + 1);
        ss >> this->client_max;
    }
}

void Config::set_root(std::string line)
{
    if (line == "")
        throw std::string("Error: No find root");
    this->root = line.substr(line.find(" ") + 1);
}

void Config::set_index(std::string line)
{
    size_t space_pos;

    if (line == "")
        return;
    space_pos = line.find(" ");
    while (space_pos != std::string::npos)
    {
        if (line.find(" ", space_pos + 1) != std::string::npos)
            this->index.push_back(line.substr(
                space_pos + 1, line.find(' ', space_pos + 1) - space_pos - 1));
        else
            this->index.push_back(line.substr(space_pos + 1));
        space_pos = line.find(' ', space_pos + 1);
    }
}

std::string Config::get_host() const
{
    return (this->host);
}

int Config::get_port() const
{
    return (this->port);
}

std::vector<std::string> Config::get_server_names() const
{
    return (this->server_names);
}

std::map<std::string, std::string> Config::get_error_pages() const
{
    return (this->error_pages);
}

int Config::get_client_max() const
{
    return (this->client_max);
}

std::string Config::get_root() const
{
    return (this->root);
}

std::vector<std::string> Config::get_index() const
{
    return (this->index);
}

std::ostream& operator<<(std::ostream& os, Config const& src)
{
    os << std::endl << "////Configuration////" << std::endl << std::endl;
    os << "Host: " << src.get_host() << std::endl;
    os << "Port: " << src.get_port() << std::endl;
    os << "Server_names:" << std::endl;
    std::vector<std::string> names = src.get_server_names();
    for (std::vector<std::string>::iterator it = names.begin();
         it != names.end(); ++it)
        os << "\t-" << *it << std::endl;
    std::map<std::string, std::string> error_pages = src.get_error_pages();
    os << "Error pages:" << std::endl;
    if (error_pages.empty() == false)
    {
        for (std::map<std::string, std::string>::const_iterator it =
                 error_pages.begin();
             it != error_pages.end(); ++it)
            std::cout << "\t-" << it->first << " " << it->second << std::endl;
    }
    os << "Client_max: " << src.get_client_max() << std::endl;
    os << "Root: " << src.get_root() << std::endl;
    os << "Index:" << std::endl;
    std::vector<std::string> index = src.get_index();
    for (std::vector<std::string>::iterator it = index.begin();
         it != index.end(); ++it)
        os << "\t-" << *it << std::endl;
    os << std::endl;
    return (os);
}