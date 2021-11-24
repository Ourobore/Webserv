#include "Location.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>

Location::Location(std::string block)
{
    this->path = block.substr(0, block.find(" "));
    set_method(search_config(block, "methods"));
    set_root(search_config(block, "root"));
    set_autoindex(search_config(block, "autoindex"));
    set_index(search_config(block, "index"));
    set_redir(search_config(block, "return"));
    set_cgi_pass(search_config(
        block, "cgi_path")); // definit l'adresser d'un serveur CGI. L'adresse
                             // peut etre specifiee sous forme de nom de domaine
                             // ou d'adresse IP et un port
    set_cgi_extension(
        search_config(block, "cgi_extension")); // définit l'extension du
                                                // fichier ou va se baser le CGI
    set_upload(search_config(block, "upload"));
}

Location::~Location()
{
}

std::string Location::search_config(std::string config, std::string key)
{
    size_t      begin;
    size_t      end;
    size_t      doublon;
    std::string line;

    begin = config.find(key, 2);
    if (begin == std::string::npos)
        return ("");
    while (config[begin - 2] != ';' && config[begin - 2] != '{' &&
           config[begin - 2] != '}' && begin != std::string::npos)
        begin = config.find(key, begin + 1);
    if (begin == std::string::npos)
        return ("");
    if (config[begin + key.size()] != ' ')
        throw std::string("Error: Missing space for " + key);
    end = config.find(";", begin);
    doublon = end;
    while ((doublon = config.find(key, doublon + 1)) != std::string::npos)
        if (config[doublon - 2] == ';' || config[doublon - 2] == '}')
            throw std::string("Error: Double " + key);
    line = config.substr(begin, (end - begin));
    return (line);
}

void Location::set_method(std::string line)
{
    size_t      space_pos;
    std::string method;

    space_pos = line.find(" ");
    if (line == "")
    {
        this->methods["GET"] = true;
        this->methods["POST"] = false;
        this->methods["DELETE"] = false;
    }
    else
    {
        this->methods["GET"] = false;
        this->methods["POST"] = false;
        this->methods["DELETE"] = false;
        space_pos = line.find(" ");
        while (space_pos != std::string::npos)
        {
            if (line.find(" ", space_pos + 1) != std::string::npos)
                method =
                    line.substr(space_pos + 1,
                                line.find(' ', space_pos + 1) - space_pos - 1);
            else
                method = line.substr(space_pos + 1);
            if (method == "GET")
                this->methods["GET"] = true;
            else if (method == "POST")
                this->methods["POST"] = true;
            else if (method == "DELETE")
                this->methods["DELETE"] = true;
            space_pos = line.find(' ', space_pos + 1);
        }
    }
    return;
}

void Location::set_root(std::string line)
{
    if (line == "")
        this->root = "";
    else
        this->root = line.substr(line.find(" ") + 1);
}

void Location::set_autoindex(std::string line)
{
    std::string tmp;

    if (line == "")
    {
        this->autoindex = "off";
        return;
    }
    tmp = line.substr(line.find(" ") + 1);
    if (tmp == "on" || tmp == "off")
        this->autoindex = tmp;
    else
        throw std::string("Error: Bad argument for autoindex");
}

void Location::set_index(std::string line)
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

void Location::set_redir(std::string line)
{
    size_t space_pos;
    size_t space_pos_bis;

    space_pos = 0;
    space_pos_bis = 0;
    if (line == "")
    {
        this->redir.first = "";
        this->redir.second = "";
    }
    else
    {
        space_pos = line.find(" ");
        space_pos_bis = line.find(" ", space_pos + 1);
        if (space_pos == std::string::npos ||
            space_pos_bis == std::string::npos ||
            line.find(" ", space_pos_bis + 1) != std::string::npos)
            throw std::string("Error: Wrong number argument for return");
        this->redir.first =
            line.substr(space_pos + 1, (space_pos_bis - space_pos - 1));
        this->redir.second = line.substr(space_pos_bis + 1);
    }
}

void Location::set_cgi_pass(std::string line)
{
    if (line == "")
        this->cgi_pass = "";
    else
        this->cgi_pass = line.substr(line.find(" ") + 1);
}

void Location::set_cgi_extension(std::string line)
{
    if (line == "")
        this->cgi_extension = "";
    else
        this->cgi_extension = line.substr(line.find(" ") + 1);
}

void Location::set_upload(std::string line)
{
    if (line == "")
        this->upload = "";
    else
        this->upload = line.substr(line.find(" ") + 1);
}

std::string Location::get_path() const
{
    return (this->path);
}

std::map<std::string, bool> Location::get_methods() const
{
    return (this->methods);
}

std::string Location::get_root() const
{
    return (this->root);
}

std::string Location::get_autoindex() const
{
    return (this->autoindex);
}

std::vector<std::string> Location::get_index() const
{
    return (this->index);
}

std::pair<std::string, std::string> Location::get_redir() const
{
    return (this->redir);
}

std::string Location::get_cgi_pass() const
{
    return (this->cgi_pass);
}

std::string Location::get_cgi_extension() const
{
    return (this->cgi_extension);
}

std::string Location::get_upload() const
{
    return (this->upload);
}

std::ostream& operator<<(std::ostream& os, Location const& src)
{
    os << "Path: " << src.get_path() << std::endl;
    os << "\tMethods:" << std::endl;
    std::map<std::string, bool> methods = src.get_methods();
    if (methods.empty() == false)
    {
        for (std::map<std::string, bool>::const_iterator it = methods.begin();
             it != methods.end(); ++it)
            os << "\t\t-" << it->first << " " << it->second << std::endl;
    }
    os << "\tRoot: " << src.get_root() << std::endl;
    os << "\tAutoindex: " << src.get_autoindex() << std::endl;
    os << "\tIndex:" << std::endl;
    std::vector<std::string> index = src.get_index();
    for (std::vector<std::string>::iterator it = index.begin();
         it != index.end(); ++it)
        os << "\t\t-" << *it << std::endl;
    os << "\tRedirection: " << src.get_redir().first << " => "
       << src.get_redir().second << std::endl;
    os << "\tCGI_pass: " << src.get_cgi_pass() << std::endl;
    os << "\tCGI_extension: " << src.get_cgi_extension() << std::endl;
    os << "\tUpload: " << src.get_upload() << std::endl;
    return (os);
}