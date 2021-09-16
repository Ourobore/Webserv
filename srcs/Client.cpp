#include "../includes/Client.hpp"

Client::Client(int domain, int type, int protocol, int port, u_long interface)
    : sock(domain, type, protocol, port, interface)
{
    int return_value = 0;
    return_value = connect(
        sock.get_fd(), reinterpret_cast<struct sockaddr*>(&sock.get_address()),
        sizeof(sock.get_address()));
    check_error(return_value, "client connection failed");
}

void Client::check_error(int value, const std::string message)
{
    if (value < 0)
    {
        std::cerr << "Error: " << message << std::endl;
        exit(EXIT_FAILURE);
    }
}

Socket& Client::get_socket()
{
    return (sock);
}