#include "Server.hpp"

Server::Server(int domain, int type, int protocol, int port, u_long interface)
    : sock(domain, type, protocol, port, interface)
{
    int return_value = 0;

    // To avoid "Already in bind" error
    int yes = 1;
    setsockopt(sock.get_fd(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    return_value =
        bind(sock.get_fd(), reinterpret_cast<sockaddr*>(&sock.get_address()),
             sizeof(sock.get_address()));
    if (return_value < 0)
        close(sock.get_fd());
    check_error(return_value, "server socket bind failed");

    return_value = listen(sock.get_fd(), 10);
    check_error(return_value, "already listening");

    poll.fd = sock.get_fd();
    poll.revents = POLLIN;
}

Server::~Server()
{
}

void Server::check_error(int value, const std::string message) const
{
    if (value < 0)
    {
        std::cerr << "Error: " << message << std::endl;
        exit(EXIT_FAILURE);
    }
}

Socket& Server::get_socket()
{
    return (sock);
}

struct pollfd& Server::get_poll()
{
    return (poll);
}