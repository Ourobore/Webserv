#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "parsing_config.hpp"
#include "utilities.hpp"

std::vector<size_t> recup_server(std::string config)
{
    int                 brackets;
    std::vector<size_t> block;
    size_t              start;
    size_t              end;
    size_t              server;

    brackets = 0;
    start = config.find('{', 0);
    server = config.find("server", 0);
    if (start == std::string::npos || server == std::string::npos ||
        start != (server + 7))
        return (block);
    for (size_t i = start; i < config.size(); i++)
    {
        if (config[i] == '{')
            brackets++;
        else if (config[i] == '}')
            brackets--;
        if (brackets == 0)
            end = i;
        if (end == i)
            break;
    }
    block.push_back(server);
    block.push_back(end);
    return (block);
}

int config_error(std::string config_final)
{
    // check the number of brackets
    int brackets;

    brackets = 0;
    for (size_t i = 0; i < config_final.size(); i++)
    {
        if (config_final[i] == '{')
            brackets++;
        else if (config_final[i] == '}')
            brackets--;
        if (brackets < 0)
            return (1);
    }
    if (brackets != 0)
        return (1);
    return (0);
}

void verif_semicolon(std::string& config_final, size_t i, int nbline)
{
    size_t j;
    size_t server_pos;

    j = i;
    if (j == 0 || (j > 0 && config_final[j - 1] == ';'))
        return;
    j--;
    if (config_final[j] == ' ' || config_final[j] == '\t' ||
        config_final[j] == '{' || config_final[j] == '}' ||
        config_final[j] == '\n')
    {
        while (j != 0 && (config_final[j] == ' ' || config_final[j] == '\t' ||
                          config_final[j] == '{' || config_final[j] == '}'))
            j--;
        if (config_final[j] != '\n' && config_final[j] != ' ' &&
            config_final[j] != '\t')
            throw std::string("Error (line " + ft::to_string(nbline) +
                              "): Problem endline");
        return;
    }
    if (j >= 5)
        server_pos = config_final.find("server", (j - 5));
    else
        throw std::string("Error (line " + ft::to_string(nbline) +
                          "): Bad character for the endline");
    if (server_pos != (j - 5))
        throw std::string("Error (line " + ft::to_string(nbline) +
                          "): Bad character for the endline");
}

void transform_config(std::string& config_final)
{
    std::size_t doubleSpace;
    int         nbline;

    nbline = 1;
    for (size_t i = 0; i < config_final.size(); i++)
    {
        if (config_final[i] == ';' && config_final[i + 1] != '\n')
            throw std::string("Error (line " + ft::to_string(nbline) +
                              "): semicolon is not end character");
        if (i > 0)
            if (config_final[i] == ';' &&
                (config_final[i - 1] == ' ' || config_final[i - 1] == '\t'))
                throw std::string("Error (line " + ft::to_string(nbline) +
                                  "): Space or tab before semicolon");
        if (config_final[i] == '\n')
        {
            verif_semicolon(config_final, i, nbline);
            nbline++;
        }
    }
    for (size_t i = 0; i < config_final.size(); i++)
    {
        if (config_final[i] == '\n' || config_final[i] == '\t')
            config_final[i] = ' ';
    }

    doubleSpace = config_final.find("  ");
    while (doubleSpace != std::string::npos)
    {
        config_final.erase(doubleSpace, 1);
        doubleSpace = config_final.find("  ");
    }
}

std::vector<Config> parse_config(const char* config)
{
    // open the file
    std::ifstream config_open;
    config_open.open(config);
    if (!config_open)
        throw std::string("Error: Config don't open");

    // read the file
    std::stringstream config_read;
    std::string       config_final;
    config_read << config_open.rdbuf();
    config_final = config_read.str();
    if (config_final.empty())
        throw std::string("Error: Config empty");

    // Transform characters and erase duplicate space
    transform_config(config_final);

    // std::cout << config_final << std::endl << std::endl;

    // check error
    if (config_error(config_final) == 1)
        throw std::string("Error: number of brackets");

    std::vector<Config> configs;
    std::vector<size_t> block;
    block = recup_server(config_final);
    if (block.empty() == true)
        throw std::string("Error: no server find");
    while (block.empty() == false)
    {
        // Create a object Config
        Config new_config(config_final.substr(block[0], (block[1] - block[0])));
        //
        configs.push_back(new_config);
        config_final.erase(block[0], block[1]);
        block = recup_server(config_final);
    }

    return (configs);
}

std::vector<Config> main_parsing_config(int argc, char** argv)
{
    std::vector<Config> configs;
    try
    {
        if (argc != 2)
            configs = parse_config("conf/double.conf");
        else
            configs = parse_config(argv[1]);
    }
    catch (std::string error)
    {
        std::cerr << error << std::endl;
        return std::vector<Config>();
    }
    for (std::vector<Config>::iterator it = configs.begin(); it < configs.end();
         it++)
    {
        std::cout << *it << std::endl;
    }
    return configs;
}