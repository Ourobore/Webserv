#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <netinet/in.h>
#include <unistd.h>

class Server
{
  private:
    Socket             sock;
    struct sockaddr_in _address;
    int                _addrlen;
    int                _port;
    int                _sock_fd;

    Server();

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    virtual ~Server();

    // Accessors
    Socket&             socket();
    struct sockaddr_in& address();
    int                 addrlen() const;
    int                 port() const;
    int                 sockfd() const;
};

#endif