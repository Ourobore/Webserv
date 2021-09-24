#include "Server.hpp"
#include <iostream>

#include "Webserv.hpp"

int main()
{
    // Create a new server
    Webserv web;
    web.create_server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
    web.create_server(AF_INET, SOCK_STREAM, 0, 9090, INADDR_ANY);

    web.start();

    return 0;
}