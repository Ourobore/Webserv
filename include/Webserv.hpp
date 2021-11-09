#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "CGIHandler.hpp"
#include "ClientHandler.hpp"
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

#define PIPEREAD 0
#define PIPEWRITE 1
#define STDIN 0
#define STDOUT 1

const int CHUNK_SIZE = 64;

class Webserv
{
  private:
    std::vector<Server>        servers;
    std::vector<ClientHandler> clients;
    std::vector<struct pollfd> pfds;

    std::map<int, std::string> res_status;

    typedef struct Response
    {
        std::string content;
        std::string content_type;
        int         code;
    } Response;

    std::string recv_data;

    // Polling
    void poll_events();
    void poll_file(ClientHandler& client, size_t& file_index);

    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);
    int  recv_all(int file_descriptor, std::string& recv_output, int flags = 0);

    // Handling requests and responses
    void request_handler(ClientHandler& client, Config& server_config);
    void response_handler(ClientHandler& client, int client_index);
    void respond(int socket_fd, Request& req, ClientHandler::Response& res);

    // Request type handling
    void handle_upload(Config& config, Request& request, ClientHandler& client);
    void handle_cgi(Config& config, Request& request, ClientHandler& client);

    // Utilities
    Server&        get_server_from_client(int client_fd);
    ClientHandler& get_client_from_file(int file_descriptor);
    int            get_poll_index(int file_descriptor);
    FileHandler*   is_file_fd(int file_descriptor);

    // Accessors
    Server&                              get_server(int server_fd);
    ClientHandler&                       get_client(int client_fd);
    std::vector<Server>::iterator        get_server_ite(int server_fd);
    std::vector<ClientHandler>::iterator get_client_ite(int client_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    void create_server(Config& config);
};

#endif