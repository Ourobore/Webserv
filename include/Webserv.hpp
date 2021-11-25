#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "CGIHandler.hpp"
#include "ClientHandler.hpp"
#include "Config.hpp"
#include "FileHandler.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "generate.hpp"
#include "utilities.hpp"
#include <csignal>

#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <vector>

#define PIPEREAD 0
#define PIPEWRITE 1
#define STDIN 0
#define STDOUT 1

const int CHUNK_SIZE = 65535; // 2^16 - 1 //32767; // 2^15 - 1
const int POLL_DELAY = 100;

class Webserv
{
  private:
    std::vector<Server>        servers;
    std::vector<ClientHandler> clients;
    std::vector<struct pollfd> pfds;

    // Polling
    void poll_events();
    void poll_file(ClientHandler& client, size_t& file_index);

    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);
    void recv_chunk(ClientHandler& client, int client_index);

    // Handling requests and responses
    void request_handler(ClientHandler& client, Config& server_config);
    void response_handler(ClientHandler& client, int client_index);
    void respond(int socket_fd, ClientHandler::Response& res);
    void wrapper_open_file(ClientHandler& client, Config& config,
                           Request& request);
    void wrapper_open_dir(ClientHandler& client, Config& config,
                          Request& request);
    void wrapper_open_error(ClientHandler& client, Config& config, int error);

    // Request type handling
    void handle_upload(Config& config, Request& request, ClientHandler& client);
    void handle_delete(Config& config, Request& request, ClientHandler& client);
    void handle_post(Config& config, Request& request, ClientHandler& client);
    void handle_cgi(Config& config, Request& request, ClientHandler& client);
    void chunk_content(std::string& content);

    // Utilities
    Server&        get_server_from_client(int client_fd, std::string& raw_host);
    ClientHandler* get_client_from_file(int file_descriptor);
    int            get_poll_index(int file_descriptor);
    std::string    get_requested_host(std::string& raw);

    FileHandler* is_file_fd(int file_descriptor);
    bool         is_cgi_input(ClientHandler& client, int file_descriptor);

    static void catch_signal(int signal);

    // Accessors
    Server&                              get_server(int server_fd);
    ClientHandler*                       get_client(int client_fd);
    std::vector<Server>::iterator        get_server_ite(int server_fd);
    std::vector<ClientHandler>::iterator get_client_ite(int client_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    void create_server(Config& config);
};

#endif