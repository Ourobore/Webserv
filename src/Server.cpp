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
}

Server::~Server()
{
}

void Server::accept()
{
    struct pollfd pfds[5];
    int           fd_count = 1;

    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;

    for (int i = 0; i < fd_count; ++i)
    {
        if (poll(pfds, 1, -1) == 0)
        {
            std::cout << "timeout" << std::endl;
        }
        else
        {
            if (pfds[0].revents & POLLIN)
            {
                std::cout << "fd " << pfds[0].fd << " is ready to read"
                          << std::endl;
                acceptfd =
                    ::accept(sockfd, reinterpret_cast<sockaddr*>(&address),
                             reinterpret_cast<socklen_t*>(&addrlen));
                Socket::check_error(acceptfd, "accept socket failed");

                recv(acceptfd, buffer, 30000, 0);
                handle();
                respond();
            }
            else
            {
                std::cout << "unexpected event occured: " << pfds[0].revents
                          << std::endl;
            }
        }
    }
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
        accept();
        std::cout << "=== Done ! ===" << std::endl;
    }
}