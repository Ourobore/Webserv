#include "Server.hpp"
#include <iostream>

int main()
{
    // Create a new server
    Server s = Server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);

    s.start();

    return 0;
}