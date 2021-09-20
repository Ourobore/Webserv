#include <iostream>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "Clients.hpp"
#include "Server.hpp"

static const int PORT = 8080;

int main()
{
    Server  server(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    Socket& server_socket = server.get_socket();

    Clients clients(server_socket.get_fd());

    char buffer[10000];

    for (;;)
    {
        int poll_count = poll(clients.get_poll(), clients.size(), -1);
        if (poll_count < 0)
        {
            std::cout << "Poll error" << std::endl;
            return (1);
        }
        for (int i = 0; i < clients.size(); ++i)
        {
            if (clients.get_poll()[i].revents & POLLIN)
            {
                if (clients.get_poll()[i].fd == server_socket.get_fd())
                {
                    int new_client = accept(server_socket.get_fd(),
                                            reinterpret_cast<sockaddr*>(
                                                &server_socket.get_address()),
                                            reinterpret_cast<socklen_t*>(
                                                &server_socket.get_address()));
                    if (new_client < 0)
                        std::cout << "In accept" << std::endl;
                    else
                    {
                        clients.add_client(new_client);
                        std::cout << "===== Client Added =====" << std::endl;
                    }
                }
                else
                {
                    int sender_fd = clients.get_poll()[i].fd;
                    int bytes_received =
                        recv(sender_fd, buffer, strlen(buffer), 0);

                    if (bytes_received <= 0)
                    {
                        if (bytes_received == 0)
                            std::cout << "No bites. Hanged up" << std::endl;
                        else
                            std::cerr << "recv() error" << std::endl;
                        close(clients.get_poll()[i].fd);
                        clients.remove_client(i);
                        std::cout << "===== Client removed =====" << std::endl;
                    }
                    else
                    {
                        for (int i = 0; i < clients.size(); ++i)
                        {
                            int receiver_fd = clients.get_poll()[i].fd;
                            if (receiver_fd != server_socket.get_fd() &&
                                receiver_fd != sender_fd)
                            {
                                if (send(receiver_fd, buffer, bytes_received,
                                         0) == -1)
                                    std::cout << "send() error" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}
