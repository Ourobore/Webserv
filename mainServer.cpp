#include "netinet/in.h"
#include "unistd.h"
#include <iostream>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

#include "includes/Server.hpp"

static const int PORT = 8080;

int main()
{
    Server  server(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    Socket& socket = server.get_socket();
    int     new_socket = 0;
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
        read(new_socket, client_output, 30000);
        std::cout << client_output << std::endl;

        std::string client_input = "### Connected to server ###";
        write(new_socket, client_input.c_str(), client_input.length() + 1);

        int valread = 1;
        while ((valread = read(new_socket, client_output, 30000)) > 0 &&
               strcmp(client_output, "exit"))
            std::cout << client_output << std::endl;

        read(new_socket, client_output, 30000);
        std::cout << client_output << std::endl;

        client_input = "### Server connection closed ###";
        write(new_socket, client_input.c_str(), client_input.length() + 1);

        std::cout << "\n-------------------------------------\n" << std::endl;
        close(new_socket);
    }
}