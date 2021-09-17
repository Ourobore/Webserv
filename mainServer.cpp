#include <iostream>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "Client.hpp"
#include "Server.hpp"

static const int PORT = 8080;

int main()
{
    Server  server(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    Socket& socket = server.get_socket();
    int     new_socket = 0;
    // std::vector<Client> clients;
    while (1)
    {
        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n"
                  << std::endl;
        if ((new_socket = accept(
                 socket.get_fd(),
                 reinterpret_cast<struct sockaddr*>(&socket.get_address()),
                 reinterpret_cast<socklen_t*>(&socket.get_address()))) < 0)
        {
            std::cout << "In accept" << std::endl;
            return (1);
        }

        char client_output[30000];
        recv(new_socket, client_output, 30000, 0);
        std::cout << client_output << std::endl;

        std::string client_input = "### Connected to server ###";
        send(new_socket, client_input.c_str(), client_input.length(), 0);

        int valread = 1;
        while ((valread = read(new_socket, client_output, 30000)) > 0 &&
               strcmp(client_output, "exit"))
            std::cout << client_output << std::endl;

        recv(new_socket, client_output, 30000, 0);
        std::cout << client_output << std::endl;

        client_input = "### Server connection closed ###";
        send(new_socket, client_input.c_str(), client_input.length(), 0);

        std::cout << "\n-------------------------------------\n" << std::endl;
        close(new_socket);
    }
}