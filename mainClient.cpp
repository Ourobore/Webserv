#include "unistd.h"
#include <iostream>
#include <netinet/in.h>

#include "includes/Client.hpp"

static const int PORT = 9090;

int main()
{
    Client  client(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    Socket& sock = client.get_socket();

    std::string server_input = "Client connected to server";
    send(sock.get_fd(), server_input.c_str(), server_input.length() + 1, 0);

    char server_output[10000];
    read(sock.get_fd(), server_output, 10000);
    std::cout << server_output << std::endl;

    std::string input;
    while (input != "exit")
    {
        getline(std::cin, input);
        send(sock.get_fd(), input.c_str(), input.length() + 1, 0);
    }
    server_input = "Client disconnected from server";
    send(sock.get_fd(), server_input.c_str(), server_input.length() + 1, 0);

    read(sock.get_fd(), server_output, 10000);
    std::cout << server_output << std::endl;
    return (0);
}