#include "Socket.hpp"

Socket::Socket(int domain, int type, int protocol, int port,
               std::string interface)
    : _ip_addr(interface), _port(port)
{
    // If the IP is localhost, we need to chang it to be network 'readable'
    std::string socket_address = interface;
    if (socket_address == "localhost")
        socket_address = "127.0.0.1";
    in_addr_t socket_ip_addr = inet_addr(socket_address.c_str());

    _address.sin_family = domain;
    _address.sin_port = htons(port);
    _address.sin_addr.s_addr = socket_ip_addr;

    _fd = socket(domain, type, protocol);
    Socket::check_error(_fd, "cannot create socket");
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
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
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

int Socket::fd() const
{
    return (_fd);
}

sockaddr_in& Socket::address()
{
    return (_address);
}

int Socket::addrlen() const
{
    return sizeof(_address);
}

std::string Socket::ip_addr() const
{
    return (_ip_addr);
}

int Socket::port() const
{
    return (_port);
}