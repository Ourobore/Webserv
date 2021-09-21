#include "Server.hpp"
#include "poll.h"

Server::Server(int domain, int type, int protocol, int port, u_long interface)
    : sock(domain, type, protocol, port, interface)
{
    address = sock.get_address();
    addrlen = sizeof(address);
    sockfd = sock.get_fd();

    // Init buffer. TODO: try to allocate dynamically ?
    std::memset(buffer, 0, 30000);

    // Bind socket to network
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    int ret = bind(sockfd, reinterpret_cast<sockaddr*>(&address), addrlen);
    Socket::check_error(ret, "server socket bind failed");

    // Let binded socket to listen for requests
    ret = ::listen(sockfd, 10);
    Socket::check_error(ret, "already listening");

    // initilialize vector of struct pollfd with listening socket
    struct pollfd new_sock = {sockfd, POLLIN, 0};
    pfds.push_back(new_sock);
}

Server::~Server()
{
}

void Server::receive()
{

    recv(acceptfd, buffer, 30000, 0);
    handle();
    respond();
}

void Server::handle()
{
    std::cout << buffer << std::endl;
}

void Server::respond()
{
    std::string       hello = "<h1>Hello from Webserv !</h1>\r\n";
    std::stringstream headers_content;

    headers_content << "HTTP/1.1 200 OK\r\n"
                    << "Connection: keep-alive\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << hello.length() << "\r\n"
                    << "\r\n";
    std::string headers = headers_content.str();

    send(acceptfd, headers.c_str(), headers.length(), 0);
    send(acceptfd, hello.c_str(), hello.length(), 0);
    // close(acceptfd);
}

Socket& Server::get_socket()
{
    return sock;
}

void Server::start()
{
    while (true)
    {
        std::cout << "=== Waiting... ===" << std::endl;

        acceptfd = ::accept(sockfd, reinterpret_cast<sockaddr*>(&address),
                            reinterpret_cast<socklen_t*>(&addrlen));
        Socket::check_error(acceptfd, "accept socket failed");
        receive();
        std::cout << "=== Done ! ===" << std::endl;
    }
}