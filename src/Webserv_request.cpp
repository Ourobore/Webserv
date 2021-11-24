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

    if (req["Method"].empty())
    {
        wrapper_open_error(client, server_config, 400);
        return;
    }

    std::pair<std::string, std::string> redir;
    if (req.location_index() != -1)
        redir = server_config.get_locations()[req.location_index()].get_redir();

    bool authorized_method = ft::access_method(server_config, req);
    if (req["Method"] == "GET" && authorized_method)
    {
        if (CGIHandler::is_cgi_file(req["URI"], req.location_index(),
                                    server_config))
            handle_cgi(server_config, req, client);
        else if (req.location_index() != -1 && redir.second != "")
        {
            client.set_location_header(redir.second);
            wrapper_open_error(client, server_config,
                               ft::to_type<int>(redir.first));
            return;
        }
        else
        {
            if (!ft::is_dir(req["URI"])) // If file
                wrapper_open_file(client, server_config, req);
            else // It is a directory
                wrapper_open_dir(client, server_config, req);
        }
    }
    else if (req["Method"] == "POST" && authorized_method)
    {
        // Need checking if form or file upload, and location. Content type?
        if (req["Body"].length() >
            server_config.get_client_max()) // Payload too large
            wrapper_open_error(client, server_config, 413);
        else
        {
            if (req["Content-Type"].find("multipart/form-data") !=
                std::string::npos)
                handle_upload(server_config, req, client);
        }
    }
    else if (req["Method"] == "DELETE" && authorized_method)
        handle_delete(server_config, req, client);
    else // Method Not Allowed
        wrapper_open_error(client, server_config, 405);
}

void Webserv::wrapper_open_file(ClientHandler& client, Config& config,
                                Request& request)
{
    FileHandler file = ft::open_file_stream(request["URI"], config);
    if (file.stream())
    {
        client.set_content_type(request["URI"], config);
        client.files().push_back(file);
        struct pollfd file_poll = {file.fd(), POLLIN, 0};
        pfds.push_back(file_poll);
    }
    else if (file.status() != 200) // If file opening failed
        wrapper_open_error(client, config, file.status());
}

void Webserv::wrapper_open_dir(ClientHandler& client, Config& config,
                               Request& request)
{
    std::string autoindex("");
    if (request.location_index() != -1)
        autoindex =
            config.get_locations()[request.location_index()].get_autoindex();

    if (autoindex == "on")
    {
        client.response().code = 200;
        client.response().content = generate::autoindex(request);
        client.response().content_type = "text/html";
        pfds[get_poll_index(client.fd())].events = POLLOUT;
    }
    else
        wrapper_open_error(client, config, 404);
}

void Webserv::wrapper_open_error(ClientHandler& client, Config& config,
                                 int error)
{
    generate::response(client, error);
    FileHandler error_page = ft::open_file_stream(
        config.get_error_pages()[ft::to_string(error)], config);

    // If error file exists
    if (!error_page.string_output().empty())
        client.response().content = error_page.string_output();

    // If Internal Server Error
    if (error_page.status() == 500)
        client.response().code = error_page.status();

    // Tell when client is ready to be written in
    pfds[get_poll_index(client.fd())].events = POLLOUT;
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
    struct ClientHandler::Response& res = client.response();

    // Send the response in a struct with headers infos
    client.set_date();
    respond(client.fd(), *client.requests().begin(), res);

    client.clear_response();
    client.requests().erase(client.requests().begin());
    pfds[client_index].events = POLLIN;
}

void Webserv::chunk_content(std::string& content)
{
    std::istringstream       iss(content);
    std::string              line;
    std::vector<std::string> lines;

    // split lines
    while (std::getline(iss, line))
    {
        if (line.find_last_of('\r') != std::string::npos)
            line.push_back('\n');
        else
        {
            line.push_back('\r');
            line.push_back('\n');
        }
        lines.push_back(line);
    }

    // append to content
    content = "";

    for (std::vector<std::string>::iterator it = lines.begin();
         it != lines.end(); ++it)
    {
        // size
        size_t len = it->length() - 2;
        if (len == 0)
            len = 1;
        content.append(ft::to_hex(len) + "\r\n");
        content.append(*it);
    }
    content.append(ft::to_string(0) + "\r\n");
}

void Webserv::respond(int socket_fd, Request& req, ClientHandler::Response& res)
{
    std::string connection = "close";
    if (res.code == 200 || res.code == 301)
        connection = "keep-alive";

    // Response headers for web browser clients
    std::stringstream headers_content;
    headers_content << "HTTP/1.1 " << res.code << " " << res_status[res.code]
                    << "\r\n"
                    << "Server: webserv/42\r\n"
                    << "Date: " << res.date << "\r\n";
    if (res.code == 301)
    {
        headers_content << "Location: " << res.location << "\r\n"
                        << "Cache-Control: no-store"
                        << "\r\n";
    }
    headers_content << "Content-Type: " << res.content_type << "\r\n";

    size_t pos = req["Accept-Encoding"].find("chunked");
    if (pos == std::string::npos)
    {
        headers_content << "Content-Length: " << res.content.length() << "\r\n";
    }
    else
    {
        headers_content << "Transfer-Encoding: chunked\r\n";
        chunk_content(res.content);
    }

    headers_content << "Connection: " << connection << "\r\n"
                    << "\r\n";

    std::string response = headers_content.str() + res.content + "\r\n";

    // send to the client through his socket
    send(socket_fd, response.c_str(), response.length(), MSG_DONTWAIT);
}
