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
        if ((is_file_fd(pfds[i].fd)) != files.end())
        {

            std::vector<FileHandler>::iterator file_it;
            file_it = is_file_fd(pfds[i].fd);
            /* Read the file, close the fd and remove it, set POLLOUT for client
             */
            if (pfds[i].revents & POLLIN)
            {
                std::string file_output = file_it->read_all();
                int         client_index = get_poll_index(file_it->fd());
                (void)client_index;
                pfds[i].events = POLLOUT;
            }
            // if (pfds[i].revents & POLLOUT) ?
        }
        else
        {
            if (pfds[i].revents & POLLIN)
            {
                // If this is a server, handle connection
                if (is_server_socket(pfds[i].fd))
                    accept_connection(pfds[i].fd);
                // Or if this is a client
                else
                {
                    char chunk[CHUNK_SIZE] = {0};
                    int  bytes_received = 0;
                    recv_data = "";
                    while ((bytes_received =
                                recv(pfds[i].fd, chunk, CHUNK_SIZE - 1,
                                     MSG_DONTWAIT)) > 0)
                    {
                        recv_data += std::string(chunk);
                        memset(chunk, 0, CHUNK_SIZE);
                    }
                    if (bytes_received == 0)
                    {
                        close(pfds[i].fd);
                        pfds.erase(pfds.begin() + i);
                        std::cout << "No bytes to read. Client disconnected "
                                     "from socket"
                                  << std::endl;
                    }
                    else
                        request_handler(pfds[i].fd);
                }
            }
            /* If fd is ready to write, write response, then set events to
               POLLIN again to be ready to read*/
            else if (pfds[i].revents & POLLOUT)
            {
                FileHandler& file = *get_file_from_client(pfds[i].fd);
                // Ou est stocké l'output du fichier?! On a besoin de séparer
                // lecture de la requete et sa réponse!
                (void)file;
            }
        }
    }
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
    // struct pollfd pfd = {servers.back().sockfd(), POLLIN, 0};
    // clients.push_back(ClientRequest(servers.back().sockfd(), 0, 0))
    // pfds.push_back(pfd);
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
    for (std::vector<Server>::iterator it = servers.begin();
         it != servers.end(); ++it)
    {
        if (it->sockfd() == server_fd)
            return (*it);
    }
    return (*servers.end()); // Be careful, for now it must be undefined if
                             // server_fd is not really a server
}
