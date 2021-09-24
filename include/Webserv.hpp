#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Request.hpp"
#include "Server.hpp"

#include <cstring>
#include <fstream>
#include <poll.h>
#include <sstream>
#include <vector>

const int BUFFER_SIZE = 30000;

class Webserv
{
  private:
    std::vector<Server>        servers;
    std::vector<struct pollfd> pfds;

    char buffer[BUFFER_SIZE];

    // Polling
    void poll_events();
    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);

    // Handling
    void handle(int i);
    void respond(int i, Request req);

    Server& get_server(int server_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    void create_server(int domain, int type, int protocol, int port,
                       u_long interface);
};

#endif