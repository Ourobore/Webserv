#include "CGIHandler.hpp"
#include "FileHandler.hpp"
#include "Webserv.hpp"
#include <exception>

int Webserv::file_to_string(const char* path, std::string& string_buffer)
{
    std::ifstream     ifs(path);
    std::stringstream buf;
    if (ifs.is_open())
    {
        buf << ifs.rdbuf();
        string_buffer = buf.str();
        return 1;
    }
    return 0;
}

// Handle clients requests
void Webserv::request_handler(int socket_fd)
{
    // Print request from client [Debug]
    std::cout << recv_data << std::endl;

    // Get server config
    Server& server = get_server_from_client(socket_fd);
    Config& config = server.config();

    // Parsing Request + rest read buffer
    Request req = Request(recv_data.c_str());
    std::memset(buffer, 0, BUFFER_SIZE);

    // Start to build the Response { content; content_type; code }
    struct Response res = {"", "", 400};

    // CGI request
    if (!req["URI"].empty() &&
        req["URI"].find(".php", req["URI"].size() - 4) != std::string::npos)
    {
        res.content = handle_cgi(config, req, socket_fd);
        res.content_type = "text/html";
        res.code = 200;
    }
    else if (!req["URI"].empty() &&
             req["URI"].find(".py", req["URI"].size() - 3) != std::string::npos)
    {
        // TODO: Try to handle a POST request through the form in
        // html/index.html
        res.content = "<p>It's a python script !</p>";
        res.code = 200;
    }
    // Simple resource request is valid
    else if (!req["URI"].empty())
        res.content = handle_uri(config, req, res);

    // 400 Bad Request
    else if (req["URI"].empty() || req["Method"].empty())
    {
        FileHandler error_404 = open_file_stream("html/400.html");
        if (error_404.stream())
            error_404.read_all(res.content);
    }

    // Send the response in a struct with headers infos
    respond(socket_fd, req, res);
}

std::string Webserv::handle_cgi(Config const& config, Request const& request,
                                int client_fd)
{
    // Just a CGI test here, need more verifications. For exemple if we are in a
    // location
    CGIHandler handler(config, request, client_fd);
    handler.execute(buffer);

    // To do: get Content-type

    // Isolate body from CGI response
    std::string string_buffer(buffer);
    int         pos = string_buffer.find("\r\n\r\n");
    string_buffer.erase(0, pos + 3);

    return (string_buffer);
}

/*
** Handle most of simple requests (non-CGI)
*/
std::string Webserv::handle_uri(Config const& config, Request const& req,
                                Response& res)
{
    std::string content;
    std::string uri = ft::strtrim(req["URI"], "/");
    std::string root = ft::strtrim(config.get_root(), "/"); // root location

    if (req["URI"] == "/") // Resolve root index
    {
        std::vector<std::string>           indexes = config.get_index();
        std::vector<std::string>::iterator it;
        for (it = indexes.begin(); it != indexes.end(); ++it)
        {
            uri = *it;
            FileHandler index = open_file_stream(root + "/" + uri);
            if (index.stream() && index.read_all(content))
            {
                res.code = 200;
                return content;
            }
        }
    }

    // Get page if we can open it, else '404 Not found'.
    // Or '500 Internal Server Error' if file opening failed ?
    FileHandler uri_file = open_file_stream(root + "/" + uri);
    if (uri_file.stream() && uri_file.read_all(content)) // 200 OK
    {
        if (req["URI"].find(".svg", req["URI"].size() - 4) != std::string::npos)
            res.content_type = "image/svg+xml";
        else if (req["URI"].find(".png", req["URI"].size() - 4) !=
                 std::string::npos)
        {
            content = "";
            file_to_string((root + "/" + uri).c_str(), content);
            res.content_type = "image/png";
        }
        res.code = 200;
    }
    switch (uri_file.status())
    {
        case 404:
        {
            // If opening fails here, should we try to open 500.html,
            // which can fail too. Non ending loop
            FileHandler error_404 = open_file_stream("html/404.html");
            if (error_404.stream() && error_404.read_all(content))
                res.code = 404;
            break;
        }
        case 500:
        {
            // Same here
            FileHandler error_500 = open_file_stream("html/500.html");
            if (error_500.stream() && error_500.read_all(content))
                res.code = 505;
            break;
        }
    }
    return content;
}

void Webserv::respond(int socket_fd, Request& req, Response& res)
{
    (void)req;
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

    std::string headers = headers_content.str();

    // send to the client through his socket
    send(socket_fd, headers.c_str(), headers.length(), 0);
    send(socket_fd, res.content.c_str(), res.content.length(), 0);

    // Should we close the connection if status code 400 or 404 ?
    // if (connection == "close")
    //    close_connection(0, i);
}
