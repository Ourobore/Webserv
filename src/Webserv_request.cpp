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
    Request req = Request(recv_data, server_config);
    client.requests().push_back(req);

    bool authorized_method = ft::access_method(server_config, req);
    if (req["Method"] == "GET" && authorized_method)
    {
        if (CGIHandler::is_cgi_file(req["URI"], req.location_index(),
                                    server_config))
            handle_cgi(server_config, req, client);
        else
        {
            FileHandler file;
            client.set_date();
            if (!ft::is_dir(req["URI"]))
                file = ft::open_file_stream(req["URI"], server_config, "r");
            if (file.stream())
            {
                client.set_content_type(req["URI"], server_config);
                client.files().push_back(file);
                struct pollfd file_poll = {file.fd(), POLLIN, 0};
                pfds.push_back(file_poll);
                return;
            }
            else // It is a directory. TODO: check autoindex
            {
                file = ft::open_file_stream(req["URI"], server_config, "r");
            }
        }
    }
    else if (req["Method"] == "POST" && authorized_method)
    {
        // Need checking if form or file upload, and location. Content type?
        handle_upload(server_config, req, client);
    }
    else if (req["Method"] == "DELETE" && authorized_method)
    {
        handle_delete(server_config, req, client);
    }
    else
        client.requests().pop_back();
}

void Webserv::handle_cgi(Config& config, Request& request,
                         ClientHandler& client)
{
    // Just a CGI test here, need more verifications. For exemple if we are
    // in a location
    CGIHandler* handler = new CGIHandler(config, request, client.fd());
    handler->launch_cgi(client, pfds, config);

    // To do: get Content-type

    // Isolate body from CGI response
    // std::string body;
    // int         pos = cgi_output.find("\r\n\r\n");
    // body.erase(0, pos + 3);

    // return (body);
}

void Webserv::response_handler(ClientHandler& client, int client_index)
{
    // Start to build the Response { content; content_type; code }
    Request&                       req = client.requests().front();
    struct ClientHandler::Response res = client.response();

    // Send the response in a struct with headers infos
    respond(client.fd(), req, res);

    client.clear_response();
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
                    << "Date: " << res.date << "\r\n"
                    << "Content-Type: " << res.content_type << "\r\n"
                    << "Content-Length: " << res.content.length() << "\r\n"
                    << "Connection: " << connection << "\r\n"
                    << "\r\n";

    std::string response = headers_content.str() + res.content;

    // send to the client through his socket
    send(socket_fd, response.c_str(), response.length(), MSG_DONTWAIT);
}
