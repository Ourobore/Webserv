#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include "utilities.hpp"

class Socket
{
  private:
    int                _fd;
    struct sockaddr_in _address;
    std::string        _ip_addr;
    int                _port;

    Socket();

  public:
    Socket(int domain, int type, int protocol, int port, std::string interface);
    virtual ~Socket();

    static void check_error(int value, const std::string message);
    void        reuse_addr();
    static void reuse_addr(int fd);

    int          fd() const;
    sockaddr_in& address();
    int          addrlen() const;
    std::string  ip_addr() const;
    int          port() const;

    static struct sockaddr_in get_socket_address(int socket_fd);
    static std::string
               get_socket_ip_address(struct sockaddr_in const& socket_address);
    static int get_socket_port(struct sockaddr_in const& socket_address);
};

#endif