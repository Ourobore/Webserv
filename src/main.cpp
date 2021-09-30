#include "Server.hpp"
#include "Webserv.hpp"
#include "parsing_config.hpp"

int main()
{
    // Parse conf/default.conf and create configs
    std::vector<Config> configs = main_parsing_config(0, NULL);
    std::cout << "configs[0].get_root(): " << configs[0].get_root()
              << std::endl;

    // Create a new server
    // TODO: Create a new server with data from configs
    Webserv web;
    web.create_server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
    web.create_server(AF_INET, SOCK_STREAM, 0, 9090, INADDR_ANY);

    web.start();

    return 0;
}
