#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Server.hpp"

#include <arpa/inet.h>
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
    std::map<int, std::string> res_status;

    char buffer[BUFFER_SIZE];

    // Polling
    void poll_events();
    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);

    // Handling request
    void        request_handler(int socket_fd);
    std::string handle_cgi(Request const& req);
    int         file_to_string(const char* path, std::string& string_buffer);
    void        respond(int socket_fd, int code, std::string content);

    // Utilities
    Server& get_server_from_client(int client_fd);
    Server& get_server(int server_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    // void create_server(int domain, int type, int protocol, int port,
    //                    u_long interface);
    void create_server(Config config);
};

#endif