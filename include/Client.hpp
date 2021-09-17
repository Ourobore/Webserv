#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Socket.hpp"
#include <sys/poll.h>

class Client
{
  private:
    int           socket_fd;
    struct pollfd poll;
    Client();

  public:
    Client(int client_socket_fd);
    ~Client();

    void           check_error(int value, const std::string message);
    int            get_socket();
    struct pollfd& get_poll();
};

#endif