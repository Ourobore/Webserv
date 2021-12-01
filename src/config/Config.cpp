#include "Config.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

Config::Config(std::string config)
{
    size_t      pos = -1;
    std::string tmp;
    while ((pos = config.find("listen", pos + 1)) != std::string::npos)
    {
        tmp = config.substr(pos - 6);
        set_host_port(search_config(tmp, "listen"));
    }
    set_server_name(search_config(config, "server_name"));
    set_error_page(config);
    set_client_max(search_config(config, "client_max_body_size"));
    set_root(search_config(config, "root"));
    set_index(search_config(config, "index"));
    set_location(config);
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
    size_t              begin;
    size_t              end;
    size_t              doublon;
    std::string         line;
    std::vector<size_t> locations;
    int                 i = 1;

    begin = config.find(key);
    if (begin == std::string::npos)
        return ("");
    while (config[begin - 2] != ';' && config[begin - 2] != '{' &&
           config[begin - 2] != '}' && begin != std::string::npos)
    {
        begin = config.find(key, begin + 1);
        if (begin == std::string::npos)
            return ("");
    }
    if (begin == std::string::npos)
        return ("");
    if (config[begin + key.size()] != ' ')
        throw std::string("Error: Missing space for " + key);
    end = config.find(";", begin);
    doublon = end;
    locations = search_location(config);
    while ((doublon = config.find(key, doublon + 1)) != std::string::npos)
    {
        for (std::vector<size_t>::iterator it = locations.begin();
             it != locations.end(); ++it)
        {
            if (doublon > *it && doublon < *(it + 1) && config[*it] != '}')
            {
                i = 0;
                break;
            }
            i++;
        }
        if ((config[doublon - 2] == ';' || config[doublon - 2] == '}') &&
            i != 0 && key != "listen")
            throw std::string("Error: Double " + key);
        i = 1;
    }
    line = config.substr(begin, (end - begin));
    return (line);
}

std::vector<size_t> Config::search_location(std::string config)
{
    std::vector<size_t> locations;
    size_t              pos = 0;
    size_t              space = 0;
    size_t              end = 0;
    size_t              another = 0;
    int                 i = 0;

    while ((pos = config.find("location", pos + 1)) != std::string::npos)
    {
        if (config[pos + 8] != ' ')
            throw std::string("Error: Missing space for location");
        space = config.find(" ", pos + 9);
        if (config[space + 1] != '{')
            throw std::string("Error: No { for location");
        end = config.find("}", space);
        another = config.find("location", pos + 1);
        if (another != std::string::npos && end > another)
            throw std::string("Error: Another location in location");
        locations.push_back(pos + 9);
        locations.push_back(end);
        i = i + 2;
    }
    return (locations);
}

void Config::set_host_port(std::string line)
{
    size_t            space_pos;
    size_t            split_pos;
    int               tmp_port = 0;
    std::stringstream ss;

    if (line == "")
    {
        this->host = "localhost";
        tmp_port = 80;
        for (std::vector<int>::iterator it = this->port.begin();
             it != this->port.end(); ++it)
        {
            if (tmp_port == *it)
                return;
        }
        this->port.push_back(tmp_port);
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
                if (this->host != "localhost")
                    throw std::string("Error: Wrong address IP");
                tmp_port = 80;
                for (std::vector<int>::iterator it = this->port.begin();
                     it != this->port.end(); ++it)
                {
                    if (tmp_port == *it)
                        return;
                }
                this->port.push_back(tmp_port);
            }
        if (tmp_port != 80)
        {
            this->host = "localhost";
            ss << line.substr(space_pos + 1);
            ss >> tmp_port;
            for (std::vector<int>::iterator it = this->port.begin();
                 it != this->port.end(); ++it)
            {
                if (tmp_port == *it)
                    return;
            }
            this->port.push_back(tmp_port);
        }
    }
    else
    {
        this->host = line.substr(space_pos + 1, (split_pos - space_pos - 1));
        if (this->host != "localhost")
            throw std::string("Error: Wrong address IP");
        ss << line.substr(split_pos + 1);
        ss >> tmp_port;
        for (std::vector<int>::iterator it = this->port.begin();
             it != this->port.end(); ++it)
        {
            if (tmp_port == *it)
                return;
        }
        this->port.push_back(tmp_port);
    }
}

void Config::set_server_name(std::string line)
{
    size_t space_pos;

    this->server_names.push_back("localhost");
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

void Config::set_location(std::string config)
{
    std::vector<size_t>           tmp;
    std::string                   block;
    std::vector<size_t>::iterator it;

    tmp = search_location(config);
    while (!tmp.empty())
    {
        it = tmp.begin();
        block = config.substr(tmp[0], (tmp[1] - tmp[0]));
        Location new_location(block);
        this->location.push_back(new_location);
        tmp.erase(tmp.begin(), tmp.begin() + 2);
    }
}

std::string Config::get_host() const
{
    return (this->host);
}

std::vector<int> Config::get_port() const
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

size_t Config::get_client_max() const
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

std::vector<Location> Config::get_locations() const
{
    return (this->location);
}

std::ostream& operator<<(std::ostream& os, Config const& src)
{
    os << std::endl << "////Configuration////" << std::endl << std::endl;
    os << "Host: " << src.get_host() << std::endl;
    os << "Port: " << std::endl;
    std::vector<int> ports = src.get_port();
    for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it)
        os << "\t-" << *it << std::endl;
    os << "Server_names:" << std::endl;
    std::vector<std::string> names = src.get_server_names();
    for (std::vector<std::string>::iterator it = names.begin();
         it != names.end(); ++it)
        os << "\t-" << *it << std::endl;
    os << "Error pages:" << std::endl;
    std::map<std::string, std::string> error_pages = src.get_error_pages();
    if (error_pages.empty() == false)
    {
        for (std::map<std::string, std::string>::const_iterator it =
                 error_pages.begin();
             it != error_pages.end(); ++it)
            os << "\t-" << it->first << " " << it->second << std::endl;
    }
    os << "Client_max: " << src.get_client_max() << std::endl;
    os << "Root: " << src.get_root() << std::endl;
    os << "Index:" << std::endl;
    std::vector<std::string> index = src.get_index();
    for (std::vector<std::string>::iterator it = index.begin();
         it != index.end(); ++it)
        os << "\t-" << *it << std::endl;
    os << "Location:" << std::endl;
    std::vector<Location> location = src.get_locations();
    for (std::vector<Location>::iterator it = location.begin();
         it != location.end(); ++it)
        os << "-" << *it << std::endl;
    os << std::endl;
    return (os);
}

void Config::set_mimetypes(std::map<std::string, std::string> mimetypes)
{
    _mimetypes = mimetypes;
}

std::map<std::string, std::string> Config::get_mimetypes() const
{
    return _mimetypes;
}

// Panic button
void Config::clean_all()
{
    host.clear();
    port.clear();
    server_names.clear();
    error_pages.clear();
    root.clear();
    index.clear();
    _mimetypes.clear();

    std::vector<Location>::iterator it_loc;
    for (it_loc = location.begin(); it_loc != location.end(); ++it_loc)
        it_loc->clean_all();
}