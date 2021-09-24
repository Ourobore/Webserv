#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class Socket
{
  private:
    Socket();

    int                fd;
    struct sockaddr_in address;

  public:
    Socket(int domain, int type, int protocol, int port, u_long interface);
    virtual ~Socket();

    static void check_error(int value, const std::string message);
    void        reuse_addr();
    static void reuse_addr(int fd);

    int          get_fd();
    sockaddr_in& get_address();
    int          get_addrlen();
};

#endif