#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "Clients.hpp"
#include "Server.hpp"

static void accept_new_connection(int server_fd, Clients& clients)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);

    if (getsockname(server_fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0)
        std::cout << "getsockname() error" << std::endl;

    int new_client =
        accept(server_fd, reinterpret_cast<sockaddr*>(&addr), &len);

    Clients::check_error(new_client, "accept() error");

    clients.add_client(new_client);
    std::cout << "+++++ Client Added +++++" << std::endl;
}

static void no_bytes_received(Clients& clients, int bytes_received,
                              int client_index)
{
    if (bytes_received == 0)
        std::cout << "No bytes. Connection hanged up" << std::endl;
    else
        std::cerr << "recv() error" << std::endl;

    close(clients.get_poll()[client_index].fd);
    clients.remove_client(client_index);
    std::cout << "----- Client removed -----" << std::endl;
}

// Search to which server is connected the sockewt client_fd
int find_server(std::vector<Server>& server, int client_fd)
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_len = sizeof(client_addr);

    if (getsockname(client_fd, reinterpret_cast<sockaddr*>(&client_addr),
                    &client_len) < 0)
        std::cout << "getsockname() error" << std::endl;

    for (size_t i = 0; i < server.size(); ++i)
    {
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        socklen_t server_len = sizeof(server_addr);

        if (getsockname(server[i].get_socket().get_fd(),
                        reinterpret_cast<sockaddr*>(&server_addr),
                        &server_len) < 0)
            std::cout << "getsockname() error" << std::endl;

        if (server_addr.sin_port == client_addr.sin_port)
            return (server[i].get_socket().get_fd());
    }
    return (0);
}

static void write_bytes(std::vector<Server>& server, Clients& clients,
                        int sender_fd, int bytes_received, char* buffer)
{
    int server_fd = find_server(server, sender_fd);

    for (int i = 0; i < clients.size(); ++i)
    {
        int receiver_fd = clients.get_poll()[i].fd;
        // Only write message if receiver is not the server or the sender
        if (find_server(server, receiver_fd) == server_fd &&
            receiver_fd != server_fd && receiver_fd != sender_fd)
        {
            if (send(receiver_fd, buffer, bytes_received, 0) == -1)
                std::cout << "send() error" << std::endl;
        }
    }
}

bool is_server_socket(std::vector<Server>& server, int socket_fd)
{
    for (size_t i = 0; i < server.size(); ++i)
    {
        if (server[i].get_socket().get_fd() == socket_fd)
            return (1);
    }
    return (0);
}

void starting_servers(int argc, char** argv, std::vector<Server>& server)
{
    int              PORT;
    std::vector<int> server_ports;
    for (int i = 1; i < argc; ++i)
    {
        if (!(PORT = strtol(argv[i], NULL, 10)))
            continue;
        if (std::find(server_ports.begin(), server_ports.end(), PORT) !=
            server_ports.end())
            continue;
        server_ports.push_back(PORT);
        server.push_back(
            Server(AF_INET, SOCK_STREAM, 0, server_ports.back(), INADDR_ANY));
    }

    if (server_ports.size() == 0)
    {
        std::cout << "No server started. Exiting" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Servers started on ports: ";
        for (size_t i = 0; i < server_ports.size(); ++i)
        {
            std::cout << server_ports[i];
            if (i != server_ports.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::vector<Server> server;
    Clients             clients;

    starting_servers(argc, argv, server);

    for (size_t i = 0; i < server.size(); ++i)
        clients.add_client(server[i].get_socket().get_fd());

    char buffer[10000];

    for (;;)
    {
        int poll_count = poll(clients.get_poll(), clients.size(), -1);
        if (poll_count < 0)
        {
            std::cout << "poll() error" << std::endl;
            return (1);
        }
        for (int i = 0; i < clients.size(); ++i)
        {
            // If revents equals POLLIN, then a client has an update for
            // a server
            if (clients.get_poll()[i].revents & POLLIN)
            {
                // If fd in pollfd is a server_fd, this server is accesible to
                // add another client
                if (is_server_socket(server, clients.get_poll()[i].fd))
                    accept_new_connection(clients.get_poll()[i].fd, clients);
                else
                {
                    int sender_fd = clients.get_poll()[i].fd;
                    std::cout << sender_fd << "   "
                              << find_server(server, sender_fd) << std::endl;
                    int bytes_received =
                        recv(sender_fd, buffer, strlen(buffer), 0);

                    if (bytes_received <= 0)
                        no_bytes_received(clients, bytes_received, i);
                    else
                        write_bytes(server, clients, sender_fd, bytes_received,
                                    buffer);
                }
            }
        }
    }
}
