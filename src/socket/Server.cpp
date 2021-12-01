#include "Server.hpp"
#include "poll.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

Server::Server(Config& config, int port)
    : sock(AF_INET, SOCK_STREAM, 0, port, config.get_host()), _config(config),
      _ip_addr(config.get_host()), _port(port)
{
    _address = sock.address();
    _addrlen = sock.addrlen();

    _sock_fd = sock.fd();
    int yes = 1;
    setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof yes);

    Socket::reuse_addr(_sock_fd);
    int ret = bind(_sock_fd, reinterpret_cast<sockaddr*>(&_address), _addrlen);
    Socket::check_error(ret, "server socket bind failed");

    // Let binded socket listen for requests
    ret = listen(_sock_fd, 10);
    Socket::check_error(ret, "already listening");
}

Server::~Server()
{
}

Socket& Server::socket()
{
    return (sock);
}

struct sockaddr_in& Server::address()
{
    return (_address);
}

int Server::addrlen() const
{
    return (_addrlen);
}

std::string Server::ip_addr() const
{
    return (_ip_addr);
}

int Server::port() const
{
    return (_port);
}

int Server::sockfd() const
{
    return (_sock_fd);
}

Config& Server::config()
{
    return _config;
}

// Panic button
void Server::clean_all()
{
    sock.clean_all();
    _config.clean_all();
    _ip_addr.clear();
}