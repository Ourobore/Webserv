#include "Server.hpp"
#include "poll.h"
#include <arpa/inet.h>

Server::Server(int domain, int type, int protocol, int port, u_long interface)
    : sock(domain, type, protocol, port, interface)
{
    address = sock.get_address();
    addrlen = sizeof(address);
    sockfd = sock.get_fd();

    // Init buffer. TODO: try to allocate dynamically ?
    std::memset(buffer, 0, 30000);

    Socket::reuse_addr(sockfd);
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
    for (size_t i = 0; i < pfds.size(); i++)
    {
        // check if someone ready to read
        if (pfds[i].revents & POLLIN)
        {
            // if this is the listener, handle connection
            if (pfds[i].fd == sockfd)
            {
                acceptfd =
                    ::accept(sockfd, reinterpret_cast<sockaddr*>(&address),
                             reinterpret_cast<socklen_t*>(&addrlen));
                Socket::check_error(acceptfd, "accept socket failed");

                std::cout << "New connection from client on socket " << acceptfd
                          << std::endl;
                // add new client socket to poll fds
                struct pollfd new_sock = {acceptfd, POLLIN, 0};
                pfds.push_back(new_sock);
            }
            // or this is a client
            else
            {
                int nbytes = recv(pfds[i].fd, buffer, 30000, 0);

                // close connection
                if (nbytes <= 0)
                {
                    std::cout << "Client deconnected from socket " << pfds[i].fd
                              << " hung up" << std::endl;
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin() + i);
                }
                // or receive data from client
                else
                {
                    handle();
                    // send a response to client with socket at i
                    respond(i);
                }
            }
        }
    }
}

void Server::handle()
{
    std::cout << buffer << std::endl;
}

void Server::respond(int i)
{
    std::string       hello = "<h1>Hello from Webserv !</h1>\r\n";
    std::stringstream headers_content;

    headers_content << "HTTP/1.1 200 OK\r\n"
                    << "Connection: keep-alive\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << hello.length() << "\r\n"
                    << "\r\n";
    std::string headers = headers_content.str();

    // send to the client through his socket
    send(pfds[i].fd, headers.c_str(), headers.length(), 0);
    send(pfds[i].fd, hello.c_str(), hello.length(), 0);
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
        // Convert vector to simple array
        struct pollfd* pfds_array = pfds.data();

        int poll_count = poll(pfds_array, pfds.size(), -1);
        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }
        receive();
    }
}
