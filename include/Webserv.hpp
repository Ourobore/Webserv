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
    std::map<int, std::string> res_status;

    char buffer[BUFFER_SIZE];

    // Polling
    void poll_events();
    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);

    // Handling request
    void        request_handler(int socket_index);
    std::string handle_cgi(Request const& req);
    int         file_to_string(const char* path, std::string& string_buffer);
    void        respond(int socket_index, int code, std::string content);

    Server& get_server(int server_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    void create_server(int domain, int type, int protocol, int port,
                       u_long interface);
};

#endif