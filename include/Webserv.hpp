#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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
    void poll_file(ClientHandler& client, int file_index);
    void poll_response(ClientHandler& client, int client_index);

    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);
    int  recv_all(int file_descriptor, std::string& recv_output, int flags = 0);

    // Handling request
    void        request_handler(ClientHandler& client, Config& server_config);
    void        response_handler(ClientHandler& client);
    std::string handle_cgi(Config const& config, Request const& request,
                           int client_fd);
    std::string handle_uri(Config const& config, Request const& req,
                           Response& res);
    // int  file_to_string(const char* path, std::string& string_buffer);
    void respond(int socket_fd, Request& req, Response& res);

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

    // Files handling
    static FileHandler* open_file_stream(std::string filename,
                                         std::string mode = "r");
    static FileHandler* open_file_stream(int         file_descriptor,
                                         std::string mode = "r");
};

#endif