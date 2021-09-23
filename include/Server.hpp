#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <poll.h>
#include <unistd.h>
#include <vector>

class Server
{
  private:
    Socket             sock;
    struct sockaddr_in address;
    int                addrlen;
    int                sockfd;

    int                        acceptfd;
    char                       buffer[30000];
    std::vector<struct pollfd> pfds;

    void poll_events();
    void handle();
    void respond(int i);

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    virtual ~Server();

    Socket& get_socket();
    void    start();
};

#endif