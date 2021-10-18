#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "FileHandler.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "utilities.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <vector>

const int BUFFER_SIZE = 30000;
const int CHUNK_SIZE = 64;

class Webserv
{
  private:
    std::vector<Server>        servers;
    std::vector<struct pollfd> pfds;
    std::map<int, std::string> res_status;

    typedef struct Response
    {
        std::string content;
        std::string content_type;
        int         code;
    } Response;

    // buffer[BUFFER_SIZE] should be replace by recv_data. Seems ok for simple
    // requests
    char        buffer[BUFFER_SIZE];
    std::string recv_data;

    // Polling
    void poll_events();
    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);

    // Handling request
    void        request_handler(int socket_fd);
    std::string handle_cgi(Config const& config, Request const& request,
                           int client_fd);
    std::string handle_uri(Config const& config, Request const& req,
                           Response& res);
    int         file_to_string(const char* path, std::string& string_buffer);
    void        respond(int socket_fd, Request& req, Response& res);

    // Files handling
    FileHandler open_file_stream(std::string filename);
    FileHandler open_file_stream(int file_descriptor);

    // Utilities
    Server& get_server_from_client(int client_fd);
    Server& get_server(int server_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    // void create_server(int domain, int type, int protocol, int port,
    //                    u_long interface);
    void create_server(Config& config);
};

#endif