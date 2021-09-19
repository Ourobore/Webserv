#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

class Server
{
  private:
    Server();

    Socket sockref;

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    ~Server();

    Socket& get_socket();
};

#endif