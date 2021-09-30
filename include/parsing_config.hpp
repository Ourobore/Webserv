#ifndef PARSING_CONFIG_HPP
#define PARSING_CONFIG_HPP

#include "Config.hpp"
#include <fstream>

std::vector<size_t> recup_server(std::string config);
int                 config_error(std::string config_final);
void                transform_config(std::string& config_final);
std::vector<Config> parse_config(const char* config);
std::vector<Config> main_parsing_config(int argc, char** argv);

#endif
