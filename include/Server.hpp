#ifndef SERVER_HPP
#define SERVER_HPP

#include "Request.hpp"
#include "Socket.hpp"
#include <poll.h>
#include <unistd.h>
#include <vector>

const int BUFFER_SIZE = 30000;

class Server
{
  private:
    Socket             sock;
    struct sockaddr_in address;
    int                addrlen;
    int                sock_fd;

    char                       buffer[BUFFER_SIZE];
    std::vector<struct pollfd> pfds;

    void poll_events();
    void handle(int i);
    void respond(int i, Request req);

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    virtual ~Server();

    Socket& get_socket();
    void    start();
};

#endif