#include "ClientHandler.hpp"
#include "FileHandler.hpp"
#include "Webserv.hpp"

Webserv::Webserv()
{
    // Init status code for server response
    res_status[200] = "OK";
    res_status[400] = "Bad Request";
    res_status[404] = "Not Found";
}

Webserv::~Webserv()
{
}

void Webserv::start()
{
    while (true)
    {
        std::cout << "=== Waiting... ===" << std::endl;
        // Convert vector to simple array
        struct pollfd* pfds_array = &(pfds[0]);

        int poll_count = poll(pfds_array, pfds.size(), -1);
        Socket::check_error(poll_count, "poll()");
        poll_events();
    }
}

void Webserv::create_server(Config& config)
{
    servers.push_back(Server(config));
    struct pollfd pfd = {servers.back().sockfd(), POLLIN, 0};
    pfds.push_back(pfd);
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
    clients.push_back(ClientHandler(accept_fd));
}

void Webserv::close_connection(int bytes_received, int client_index)
{
    if (bytes_received == 0)
        std::cout << "No bytes to read. Client disconnected from socket "
                  << pfds[client_index].fd << std::endl;
    else
        std::cout << "recv() error" << std::endl;
    close(pfds[client_index].fd);
    clients.erase(get_client_ite(pfds[client_index].fd));
    pfds.erase(pfds.begin() + client_index);
}

/* Return the server from which the client is connected */
Server& Webserv::get_server_from_client(int client_fd)
{
    // Get client port and client IP address from it's fd (in in_addr_t type)
    struct sockaddr_in client_address = Socket::get_socket_address(client_fd);
    int                client_port = ntohs(client_address.sin_port);
    in_addr_t          client_ip_addr = client_address.sin_addr.s_addr;

    // For each server, check if server port and server IP address correspond to
    // the ones of the client
    std::vector<Server>::iterator it;
    for (it = servers.begin(); it != servers.end(); ++it)
    {
        // If the IP is localhost, we need to chang it to be network 'readable'
        std::string server_address = it->ip_addr();
        if (server_address == "localhost")
            server_address = "127.0.0.1";

        in_addr_t server_ip_addr = inet_addr(server_address.c_str());
        if (client_port == it->port() && client_ip_addr == server_ip_addr)
            return (*it);
    }
    return (servers.back());
    // We should not arrive here, but right now we can't
    // determine if there is an error. Return a pointer?
}

Server& Webserv::get_server(int server_fd)
{
    std::vector<Server>::iterator it;

    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->sockfd() == server_fd)
            return (*it);
    }
    return (*servers.end()); // Be careful, for now it must be undefined if
                             // server_fd is not really a server
}

ClientHandler& Webserv::get_client(int client_fd)
{
    std::vector<ClientHandler>::iterator it;

    if (is_file_fd(client_fd))
        return (get_client_from_file(client_fd));

    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->fd() == client_fd)
            return (*it);
    }
    return (*clients.end()); // Be careful, for now it must be undefined if
                             // client_fd is not really a client
}

std::vector<Server>::iterator Webserv::get_server_ite(int server_fd)
{
    std::vector<Server>::iterator it;

    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->sockfd() == server_fd)
            return (it);
    }
    return (servers.end());
}

std::vector<ClientHandler>::iterator Webserv::get_client_ite(int client_fd)
{
    std::vector<ClientHandler>::iterator it;

    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->fd() == client_fd)
            return (it);
    }
    return (clients.end());
}
