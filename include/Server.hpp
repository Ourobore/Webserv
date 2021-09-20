#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include <poll.h>
#include <unistd.h>

class Server
{
  private:
    Socket        sock;
    struct pollfd poll;
    Server();

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    ~Server();

    static void    check_error(int value, const std::string message);
    Socket&        get_socket();
    struct pollfd& get_poll();
};

#endif