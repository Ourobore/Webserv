#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

class Server
{
  private:
    Socket sock;
    Server();

  public:
    Server(int domain, int type, int protocol, int port, u_long interface);
    ~Server();

    void    check_error(int value, const std::string message);
    Socket& get_socket();
};

#endif