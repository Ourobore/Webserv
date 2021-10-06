#include "Server.hpp"
#include "Webserv.hpp"
#include "parsing_config.hpp"

#include "CGIHandler.hpp"

int main(int argc, char** argv)
{
    // Parse conf/default.conf and create configs
    std::vector<Config> configs = main_parsing_config(argc, argv);

    // Create a new server
    // TODO: Create a new server with data from configs
    Webserv web;

    std::vector<Config>::iterator it;
    for (it = configs.begin(); it != configs.end(); ++it)
        web.create_server(AF_INET, SOCK_STREAM, 0, it->get_port(), INADDR_ANY);

    web.start();

    return 0;
}
