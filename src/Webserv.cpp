#include "Webserv.hpp"
#include <sys/_types/_socklen_t.h>

Webserv::Webserv()
{
    // Init buffer. TODO: try to allocate dynamically ?
    std::memset(buffer, 0, BUFFER_SIZE);
}

Webserv::~Webserv()
{
}

bool Webserv::is_server_socket(int socket_fd)
{
    for (std::vector<Server>::iterator it = servers.begin();
         it != servers.end(); ++it)
    {
        if (it->sockfd() == socket_fd)
            return (true);
    }
    return (false);
}

void Webserv::accept_connection(int server_fd)
{
    Server& server = get_server(server_fd);
    int     address_length = server.addrlen();

    int accept_fd =
        accept(server_fd, reinterpret_cast<sockaddr*>(&server.address()),
               reinterpret_cast<socklen_t*>(&address_length));
    Socket::check_error(accept_fd, "accept() socket creation failed");

    std::cout << "New connection from client on socket " << accept_fd
              << std::endl;
    // Add new client socket to the struct pollfd
    struct pollfd new_sock = {accept_fd, POLLIN, 0};
    pfds.push_back(new_sock);
}

void Webserv::close_connection(int bytes_received, int client_index)
{
    if (bytes_received == 0)
        std::cout << "No bytes to read. Client disconnected from socket "
                  << pfds[client_index].fd << std::endl;
    else
        std::cout << "recv() error" << std::endl;
    close(pfds[client_index].fd);
    pfds.erase(pfds.begin() + client_index);
}

void Webserv::poll_events()
{
    for (size_t i = 0; i < pfds.size(); i++)
    {
        // Check if someone has something to read
        if (pfds[i].revents & POLLIN)
        {
            // If this is a server, handle connection
            if (is_server_socket(pfds[i].fd))
                accept_connection(pfds[i].fd);
            // Or if this is a client
            else
            {
                int bytes_received = recv(pfds[i].fd, buffer, BUFFER_SIZE, 0);
                // If no bytes received, then close connection
                if (bytes_received <= 0)
                    close_connection(bytes_received, i);
                // Or receive data from client
                else
                {
                    handle(i);
                    // send a response to client with socket at i
                }
            }
        }
    }
}

void Webserv::handle(int i)
{
    std::cout << buffer << std::endl;
    Request new_req = Request(buffer);
    respond(i, new_req);
}

void Webserv::respond(int i, Request req)
{
    // handle url
    if (req.url == "/")
    {
        req.url = "index.html";
    }
    std::ifstream     ifs("html/" + req.url);
    std::stringstream buf;
    std::string       content;
    if (ifs.is_open())
    {
        buf << ifs.rdbuf();
        content = buf.str();
    }
    else
    {
        content = "<h1>Hello from Webserv !</h1>";
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

void Webserv::start()
{
    while (true)
    {
        std::cout << "=== Waiting... ===" << std::endl;
        // Convert vector to simple array
        struct pollfd* pfds_array = &(pfds[0]);

        int poll_count = poll(pfds_array, pfds.size(), -1);
        Socket::check_error(poll_count, "poll");
        poll_events();
    }
}

void Webserv::create_server(int domain, int type, int protocol, int port,
                            u_long interface)
{
    servers.push_back(Server(domain, type, protocol, port, interface));
    struct pollfd pfd = {servers.back().sockfd(), POLLIN, 0};
    pfds.push_back(pfd);
}

Server& Webserv::get_server(int server_fd)
{
    for (std::vector<Server>::iterator it = servers.begin();
         it != servers.end(); ++it)
    {
        if (it->sockfd() == server_fd)
            return (*it);
    }
    return (*servers.end()); // Be careful, for now it muste be undefined if
                             // server_fd is not really a server
}