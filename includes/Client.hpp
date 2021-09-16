#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Socket.hpp"

class Client
{
  private:
    Socket sock;
    Client();

  public:
    Client(int domain, int type, int protocol, int port, u_long interface);
    void    check_error(int value, const std::string message);
    Socket& get_socket();
};

#endif