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
    // int     new_socket = 0;

    Clients        clients(server_socket.get_fd());
    struct pollfd* pfds = clients.get_poll();

    char buffer[10000];

    for (;;)
    {
        int poll_count = poll(pfds, clients.size(), -1);
        if (poll_count < 0)
        {
            std::cout << "Poll error" << std::endl;
            return (1);
        }

        for (int i = 0; i < clients.size(); ++i)
        {
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == server_socket.get_fd())
                {
                    int new_client = accept(server_socket.get_fd(),
                                            reinterpret_cast<sockaddr*>(
                                                &server_socket.get_address()),
                                            reinterpret_cast<socklen_t*>(
                                                &server_socket.get_address()));
                    if (new_client < 0)
                        std::cout << "In accept" << std::endl;
                    else
                        clients.add_client(new_client);
                }
                else
                {
                    int sender_fd = pfds[i].fd;
                    int bytes_received =
                        recv(sender_fd, buffer, strlen(buffer), 0);

                    if (bytes_received <= 0)
                    {
                        if (bytes_received == 0)
                            std::cout << "No bites. Hanged up" << std::endl;
                        else
                            std::cerr << "recv error" << std::endl;
                        close(pfds[i].fd);
                        clients.remove_client(i);
                        std::cout << "Client removed" << std::endl;
                    }
                    else
                    {
                        for (int i = 0; i < clients.size(); ++i)
                        {
                            int receiver_fd = pfds[i].fd;
                            if (receiver_fd != server_socket.get_fd() &&
                                receiver_fd != sender_fd)
                            {
                                if (send(receiver_fd, buffer, bytes_received,
                                         0) == -1)
                                    std::cout << "Send() error" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
        std::cout << "\n+++++++ Waiting for new connection ++++++++\n"
                  << std::endl;

        if ((new_socket = accept(
                 server_socket.get_fd(),
                 reinterpret_cast<sockaddr*>(&server_socket.get_address()),
                 reinterpret_cast<socklen_t*>(&server_socket.get_address())))
   < 0)
        {
            std::cout << "In accept" << std::endl;
            return (1);
        }
        // clients.push_back(new_socket);

        char client_output[30000];
        recv(new_socket, client_output, 30000, 0);
        std::cout << client_output << std::endl;

        std::string client_input = "### Connected to server ###";
        send(new_socket, client_input.c_str(), client_input.length(),
   0);

        int valread = 1;
        while ((valread = read(new_socket, client_output, 30000)) > 0 &&
               strcmp(client_output, "exit"))
            std::cout << client_output << std::endl;

        recv(new_socket, client_output, 30000, 0);
        std::cout << client_output << std::endl;

        client_input = "### Server connection closed ###";
        send(new_socket, client_input.c_str(), client_input.length(),
   0);

        std::cout << "\n-------------------------------------\n" <<
   std::endl; close(new_socket);
    }
        */
