#ifndef CLIENTSIDE_HPP
#define CLIENTSIDE_HPP

#include "Socket.hpp"

// This class will disappear
// We don't need a Socket object in our implementation of the client in Webserv
// Just a socket_fd from accept()

class ClientSide
{
  private:
    Socket sock;
    ClientSide();

  public:
    ClientSide(int domain, int type, int protocol, int port, u_long interface);
    ~ClientSide();

    void    check_error(int value, const std::string message);
    Socket& get_socket();
};

#endif