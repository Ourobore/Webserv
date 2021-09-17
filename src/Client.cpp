#include "Client.hpp"

Client::Client(int client_socket_fd) : socket_fd(client_socket_fd)
{
}

Client::~Client()
{
}

void Client::check_error(int value, const std::string message)
{
    if (value < 0)
    {
        std::cerr << "Error: " << message << std::endl;
        exit(EXIT_FAILURE);
    }
}

int Client::get_socket()
{
    return (socket_fd);
}

struct pollfd& Client::get_poll()
{
    return (poll);
}