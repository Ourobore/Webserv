#include "Client.hpp"

Client::Client(int domain, int type, int protocol, int port, u_long interface)
    : sockref(domain, type, protocol, port, interface)
{
    int return_value = 0;
    return_value =
        connect(sockref.get_fd(),
                reinterpret_cast<struct sockaddr*>(&sockref.get_address()),
                sizeof(sockref.get_address()));
    Socket::check_error(return_value, "client connection failed");
}

Socket& Client::get_socket()
{
    return (sockref);
}
