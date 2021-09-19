#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>

class Server
{
  private:
    Server();

    Socket             sock;
    struct sockaddr_in address;
    int                addrlen;
    int                sockfd;

    int  acceptfd;
    char buffer[30000];
    void accept();
    void handle();
    void respond();

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    virtual ~Server();

    Socket& get_socket();
    void    start();
};

#endif