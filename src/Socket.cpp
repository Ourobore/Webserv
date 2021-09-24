#include "Socket.hpp"

Socket::Socket(int domain, int type, int protocol, int port, u_long interface)
{

    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);

    fd = socket(domain, type, protocol);
    Socket::check_error(fd, "cannot create socket");
}

Socket::~Socket()
{
}

void Socket::check_error(int val, const std::string msg)
{
    if (val < 0)
    {
        std::string err = "Error: " + msg;
        perror(err.c_str());
        exit(EXIT_FAILURE);
    }
}

void Socket::reuse_addr()
{
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void Socket::reuse_addr(int fd)
{
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }
}

int Socket::get_fd()
{
    return (fd);
}

sockaddr_in& Socket::get_address()
{
    return (address);
}

int Socket::get_addrlen()
{
    return sizeof(address);
}