#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class Socket
{
  private:
    int                fd;
    struct sockaddr_in address;
    Socket();

  public:
    Socket(int domain, int type, int protocol, int port, u_long interface);
    ~Socket();

    void         check_error(int value, const std::string message);
    int          get_fd();
    sockaddr_in& get_address();
};

#endif