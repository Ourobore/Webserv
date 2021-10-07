#include "Server.hpp"
#include "poll.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

// Server::Server(int domain, int type, int protocol, int port,
//                std::string interface)
//     : sock(domain, type, protocol, port, interface), _ip_addr(interface),
//       _port(port)
// {
//     _address = sock.address();
//     _addrlen = sock.addrlen(); // To check later if the size change and must
//                                // do a reevaluation later
//     _sock_fd = sock.fd();

//     Socket::reuse_addr(_sock_fd);
//     int ret = bind(_sock_fd, reinterpret_cast<sockaddr*>(&_address),
//     _addrlen); Socket::check_error(ret, "server socket bind failed");

//     // Let binded socket to listen for requests
//     ret = listen(_sock_fd, 10);
//     Socket::check_error(ret, "already listening");

//     // initilialize vector of struct pollfd with listening socket
//     // struct pollfd new_sock = {sock_fd, POLLIN, 0};
//     // pfds.push_back(new_sock);
// }

Server::Server(Config& config)
    : sock(AF_INET, SOCK_STREAM, 0, config.get_port(), config.get_host()),
      _ip_addr(config.get_host()), _port(config.get_port()), _config(config)
{
    _address = sock.address();
    _addrlen = sock.addrlen(); // To check later if the size change and must
                               // do a reevaluation later
    _sock_fd = sock.fd();

    Socket::reuse_addr(_sock_fd);
    int ret = bind(_sock_fd, reinterpret_cast<sockaddr*>(&_address), _addrlen);
    Socket::check_error(ret, "server socket bind failed");

    // Let binded socket to listen for requests
    ret = listen(_sock_fd, 10);
    Socket::check_error(ret, "already listening");

    // initilialize vector of struct pollfd with listening socket
    // struct pollfd new_sock = {sock_fd, POLLIN, 0};
    // pfds.push_back(new_sock);
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