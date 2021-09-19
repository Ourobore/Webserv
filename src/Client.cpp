#include "Client.hpp"

Client::Client(int domain, int type, int protocol, int port, u_long interface)
    : sock(domain, type, protocol, port, interface)
{
    int ret = 0;
    ret = connect(sock.get_fd(),
                  reinterpret_cast<struct sockaddr*>(&sock.get_address()),
                  sizeof(sock.get_address()));
    Socket::check_error(ret, "client connection failed");
}

Socket& Client::get_socket()
{
    return sock;
}
