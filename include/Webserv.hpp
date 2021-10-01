#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
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
    std::vector<Config>        configs;
    std::vector<struct pollfd> pfds;

    char buffer[BUFFER_SIZE];

    // Polling
    void poll_events();
    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);

    // Handling request
    void handle(int socket_index);
    void respond(int socket_index, std::string content);

    Server& get_server(int server_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    void create_server(int domain, int type, int protocol, int port,
                       u_long interface);
};

#endif