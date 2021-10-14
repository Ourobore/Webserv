#include "CGIHandler.hpp"
#include "Server.hpp"
#include "Webserv.hpp"
#include "parsing_config.hpp"
#include "utilities.hpp"
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char** argv)
{
    // Parse conf/default.conf and create configs
    std::vector<Config> configs = main_parsing_config(argc, argv);
    if (configs.empty())
        return (-1);

    // Create a new server
    Webserv web;

    std::vector<Config>::iterator it;
    for (it = configs.begin(); it != configs.end(); ++it)
        web.create_server(*it);

    web.start();

    return 0;
}
