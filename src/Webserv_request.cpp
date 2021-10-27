#include "CGIHandler.hpp"
#include "ClientHandler.hpp"
#include "FileHandler.hpp"
#include "Webserv.hpp"
#include <exception>

// Handle clients requests
void Webserv::request_handler(ClientHandler& client, Config& server_config)
{
    // Print request from client [Debug]
    std::cout << recv_data << std::endl;

    // Parsing Request + add request to ClientHandler object
    Request req = Request(recv_data.c_str());

    client.requests().push_back(req); // Will need to delete when executed,
                                      // surely will be front() request

    std::string uri = ft::strtrim(req["URI"], "/");
    std::string root =
        ft::strtrim(server_config.get_root(), "/"); // root location

    // Need parsing req["Root"] and req["URI"]
    FileHandler* file = open_file_stream(root + "/" + uri);

    client.files().push_back(*file);
    struct pollfd file_poll = {file->fd(), 1, 0};
    pfds.push_back(file_poll);
}

std::string Webserv::handle_cgi(Config const& config, Request const& request,
                                int client_fd)
{
    // Just a CGI test here, need more verifications. For exemple if we are in a
    // location
    CGIHandler  handler(config, request, client_fd);
    std::string cgi_output = handler.execute();

    // To do: get Content-type

    // Isolate body from CGI response
    std::string body(cgi_output);
    int         pos = cgi_output.find("\r\n\r\n");
    body.erase(0, pos + 3);

    return (body);
}

void Webserv::response_handler(ClientHandler& client, int client_index)
{
    // Start to build the Response { content; content_type; code }
    Request&                       req = client.requests().front();
    struct ClientHandler::Response res = client.response();

    // Send the response in a struct with headers infos
    respond(client.fd(), req, res);

    client.requests().erase(client.requests().begin());
    pfds[client_index].events = POLLIN;
}

void Webserv::respond(int socket_fd, Request& req, ClientHandler::Response& res)
{
    (void)req; // Maybe remove, useful ?
    std::string connection = "close";
    if (res.code == 200)
        connection = "keep-alive";

    // Response headers for web browser clients
    std::stringstream headers_content;
    headers_content << "HTTP/1.1 " << res.code << " " << res_status[res.code]
                    << "\r\n"
                    << "Server: webserv/42\r\n"
                    << "Date: \r\n"
                    << "Content-Type: " << res.content_type << "\r\n"
                    << "Content-Length: " << res.content.length() << "\r\n"
                    << "Connection: " << connection << "\r\n"
                    << "\r\n";

    std::string response = headers_content.str() + res.content;

    // send to the client through his socket
    send(socket_fd, response.c_str(), response.length(), 0);
}
