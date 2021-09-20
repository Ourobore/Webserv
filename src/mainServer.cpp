#include <iostream>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "Clients.hpp"
#include "Server.hpp"

static int PORT;

static void accept_new_connection(Server& server, Clients& clients)
{
    Socket& server_socket = server.get_socket();

    int new_client =
        accept(server_socket.get_fd(),
               reinterpret_cast<sockaddr*>(&server_socket.get_address()),
               reinterpret_cast<socklen_t*>(&server_socket.get_address()));

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

static void write_bytes(Server& server, Clients& clients, int sender_fd,
                        int bytes_received, char* buffer)
{
    Socket& server_socket = server.get_socket();

    for (int i = 0; i < clients.size(); ++i)
    {
        int receiver_fd = clients.get_poll()[i].fd;
        // Only write message if receiver is not the server or the sender
        if (receiver_fd != server_socket.get_fd() && receiver_fd != sender_fd)
        {
            if (send(receiver_fd, buffer, bytes_received, 0) == -1)
                std::cout << "send() error" << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    // Port selection
    if (argc == 2 && (PORT = strtol(argv[1], NULL, 10)))
        std::cout << "Server started on port " << PORT << std::endl;
    else
    {
        std::cout << "Server port was not specified or was not valid. Default "
                     "port is set at 8080"
                  << std::endl;
        PORT = 8080;
    }

    Server  server(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    Socket& server_socket = server.get_socket();

    Clients clients(server_socket.get_fd());

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
            // If revents equals POLLIN, then the clients has an update for the
            // server
            if (clients.get_poll()[i].revents & POLLIN)
            {
                // If fd in pollfd is server_fd, server is accesible to add
                // another client
                if (clients.get_poll()[i].fd == server_socket.get_fd())
                    accept_new_connection(server, clients);
                else
                {
                    int sender_fd = clients.get_poll()[i].fd;
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
