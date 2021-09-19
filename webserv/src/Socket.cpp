#include "Socket.hpp"

Socket::Socket()
{
}

Socket::Socket(int domain, int type, int protocol, int port, u_long interface)
{

    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);

    fd = socket(domain, type, protocol);
    check_error(fd, "cannot create socket");
}

Socket::~Socket()
{
}

void Socket::check_error(int val, const std::string msg)
{
    if (val < 0)
    {
        std::cerr << "Error: " << msg << std::endl;
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
