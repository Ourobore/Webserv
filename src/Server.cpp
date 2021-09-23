#include "Server.hpp"
#include "poll.h"
#include <cstring>
#include <fstream>
#include <sstream>

Server::Server(int domain, int type, int protocol, int port, u_long interface)
    : sock(domain, type, protocol, port, interface)
{
    address = sock.get_address();
    addrlen = sizeof(address); // To check later if the size change
    sock_fd = sock.get_fd();

    // Init buffer. TODO: try to allocate dynamically ?
    std::memset(buffer, 0, BUFFER_SIZE);

    Socket::reuse_addr(sock_fd);
    int ret = bind(sock_fd, reinterpret_cast<sockaddr*>(&address), addrlen);
    Socket::check_error(ret, "server socket bind failed");

    // Let binded socket to listen for requests
    ret = ::listen(sock_fd, 10);
    Socket::check_error(ret, "already listening");

    // initilialize vector of struct pollfd with listening socket
    struct pollfd new_sock = {sock_fd, POLLIN, 0};
    pfds.push_back(new_sock);
}

Server::~Server()
{
}

void Server::poll_events()
{
    for (size_t i = 0; i < pfds.size(); i++)
    {
        // check if someone ready to read
        if (pfds[i].revents & POLLIN)
        {
            // if this is the listener, handle connection
            if (pfds[i].fd == sock_fd)
            {
                int accept_fd =
                    accept(sock_fd, reinterpret_cast<sockaddr*>(&address),
                           reinterpret_cast<socklen_t*>(&addrlen));
                Socket::check_error(accept_fd, "accept socket failed");

                std::cout << "New connection from client on socket "
                          << accept_fd << std::endl;
                // add new client socket to poll fds
                struct pollfd new_sock = {accept_fd, POLLIN, 0};
                pfds.push_back(new_sock);
            }
            // or this is a client
            else
            {
                int nbytes = recv(pfds[i].fd, buffer, BUFFER_SIZE, 0);

                // close connection
                if (nbytes <= 0)
                {
                    if (nbytes == 0)
                        std::cout << "Client disconnected from socket "
                                  << pfds[i].fd << std::endl;
                    else
                        std::cout << "recv() error" << std::endl;
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin() + i);
                }
                // or receive data from client
                else
                {
                    handle(i);
                    // send a response to client with socket at i
                }
            }
        }
    }
}

void Server::handle(int i)
{
    std::cout << buffer << std::endl;
    Request new_req = Request(buffer);
    std::memset(buffer, 0, BUFFER_SIZE);
    respond(i, new_req);
}

void Server::respond(int i, Request req)
{
    // handle url
    std::stringstream buf;
    std::string       content = "No content\r\n";

    if (req.url == "/")
        req.url = "index.html";

    std::ifstream ifs(("html/" + req.url).c_str());

    if (ifs.is_open())
    {
        buf << ifs.rdbuf();
        content = buf.str() + "\r\n";
    }

    // Response headers for web browser clients
    std::stringstream headers_content;
    headers_content << "HTTP/1.1 200 OK\r\n"
                    << "Connection: keep-alive\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << content.length() << "\r\n"
                    << "\r\n";
    std::string headers = headers_content.str();

    // send to the client through his socket

    send(pfds[i].fd, headers.c_str(), headers.length(), 0);
    send(pfds[i].fd, content.c_str(), content.length(), 0);
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
        struct pollfd* pfds_array = &pfds[0];

        int poll_count = poll(pfds_array, pfds.size(), -1);
        Socket::check_error(poll_count, "poll");
        poll_events();
    }
}
