#include "ClientHandler.hpp"
#include "FileHandler.hpp"
#include "Webserv.hpp"

Webserv* webserv;
bool     stop_server = false;

Webserv::Webserv()
{
}

Webserv::~Webserv()
{
}

void Webserv::start()
{
    webserv = this;
    while (!stop_server)
    {
        // std::cout << "=== Waiting... ===" << std::endl;
        signal(SIGINT, catch_signal);

        // Convert vector to simple array
        struct pollfd* pfds_array = &(pfds[0]);

        // Server core: tells us if there is events on watched FD
        int poll_count = poll(pfds_array, pfds.size(), POLL_DELAY);
        if (poll_count < 0)
        {
            Socket::check_error(poll_count, "poll()", false);
            stop_server = true;
        }

        // Process events
        poll_events();
    }
}

void catch_signal(int signal)
{
    std::cout << std::endl
              << "Signal called (" << strsignal(signal) << "). Stopping..."
              << std::endl;

    webserv->clean_all();
    stop_server = true;
}

void Webserv::create_server(Config& config)
{
    std::vector<int> ports = config.get_port();

    for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it)
    {
        servers.push_back(Server(config, *it));
        struct pollfd pfd = {servers.back().sockfd(), POLLIN, 0};
        pfds.push_back(pfd);
        std::cout << "Create " << config.get_server_names()[0]
                  << " server at port " << *it << std::endl;
    }
}

void Webserv::clean_all()
{
    std::vector<Server>::iterator it_serv;
    for (it_serv = servers.begin(); it_serv != servers.end(); ++it_serv)
        it_serv->clean_all();

    std::vector<ClientHandler>::iterator it_client;
    for (it_client = clients.begin(); it_client != clients.end(); ++it_client)
        it_client->clean_all();

    pfds.clear();
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
    // fcntl(accept_fd, F_SETFL, O_NONBLOCK);
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

std::string Webserv::get_requested_host(std::string& raw)
{
    std::string requested_host = "";

    size_t pos = raw.find("\nHost:");
    if (pos != std::string::npos)
    {
        requested_host = raw.substr(pos + 7);
        pos = requested_host.find_first_of(":\r");
        if (pos != std::string::npos)
            requested_host.erase(pos);
    }
    return requested_host;
}

/* Return the server from which the client is connected */
Server& Webserv::get_server_from_client(int          client_fd,
                                        std::string& raw_hostname)
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
        if (server_address == "localhost" || server_address == "0.0.0.0")
            server_address = "127.0.0.1";

        in_addr_t server_ip_addr = inet_addr(server_address.c_str());
        if (client_port == it->port() && client_ip_addr == server_ip_addr)
        {
            std::string requested_host = get_requested_host(raw_hostname);
            std::vector<std::string> server_names =
                it->config().get_server_names();

            std::vector<std::string>::iterator sn;
            for (sn = server_names.begin(); sn != server_names.end(); ++sn)
            {
                if (*sn == requested_host)
                    return (*it);
            }
        }
    }
    // Return default_server (first server block) if server_name doesn't match
    // any
    return (servers.front());
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

ClientHandler* Webserv::get_client(int client_fd)
{
    std::vector<ClientHandler>::iterator it;

    if (is_file_fd(client_fd))
        return (get_client_from_file(client_fd));

    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->fd() == client_fd)
            return (&*it);
    }
    return (NULL);
}

ClientHandler* Webserv::get_client_from_file(int file_descriptor)
{
    std::vector<ClientHandler>::iterator client_it;

    for (client_it = clients.begin(); client_it != clients.end(); ++client_it)
    {
        std::vector<FileHandler>::iterator file_it;
        for (file_it = client_it->files().begin();
             file_it != client_it->files().end(); ++file_it)
            if (file_it->fd() == file_descriptor)
                return (&*client_it);
    }
    return (NULL);
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
