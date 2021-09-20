#include "unistd.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>

#include "ClientSide.hpp"

static int PORT;

int main(int argc, char** argv)
{
    if (argc == 2 && (PORT = strtol(argv[1], NULL, 10)))
        std::cout << "Client started on port " << PORT << std::endl;
    else
    {
        PORT = 8080;
        std::cout << "Client port was not specified or not valid. Default"
                     " port is set at 8080"
                  << std::endl;
    }

    ClientSide client(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    Socket&    sock = client.get_socket();

    std::string server_input = "+++++ Client connected to server +++++";
    send(sock.get_fd(), server_input.c_str(), server_input.length(), 0);

    char server_output[30000];
    // recv(sock.get_fd(), server_output, 30000, 0);
    // std::cout << server_output << std::endl;

    std::string input;
    while (input != "exit")
    {
        while (recv(sock.get_fd(), server_output, strlen(server_output), 0) > 0)
            std::cout << server_output << std::endl;

        getline(std::cin, input);
        send(sock.get_fd(), input.c_str(), input.length(), 0);
    }
    server_input = "----- Client disconnected from server ------";
    send(sock.get_fd(), server_input.c_str(), server_input.length(), 0);
    close(sock.get_fd());

    // recv(sock.get_fd(), server_output, 30000, 0);
    // std::cout << server_output << std::endl;
    return (0);
}