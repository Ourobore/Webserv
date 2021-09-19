#include "Server.hpp"

Server::Server(int domain, int type, int protocol, int port, u_long interface)
    : sockref(domain, type, protocol, port, interface)
{
    int ret = bind(sockref.get_fd(),
                   reinterpret_cast<sockaddr*>(&sockref.get_address()),
                   sizeof(sockref.get_address()));
    Socket::check_error(ret, "server socket bind failed");

    ret = listen(sockref.get_fd(), 3);
    Socket::check_error(ret, "already listening");
}

Server::~Server()
{
}

Socket& Server::get_socket()
{
    return (sockref);
}