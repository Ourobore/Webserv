#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <netinet/in.h>
#include <string>
#include <unistd.h>

class Server
{
  private:
    Socket             sock;
    struct sockaddr_in _address;
    int                _addrlen;
    std::string        _ip_addr;
    int                _port;
    int                _sock_fd;

    Server();

  public:
    Server(int domain, int type, int protocol, int port, std::string interface);
    virtual ~Server();

    // Accessors
    Socket&             socket();
    struct sockaddr_in& address();
    int                 addrlen() const;
    std::string         ip_addr() const;
    int                 port() const;
    int                 sockfd() const;
};

#endif