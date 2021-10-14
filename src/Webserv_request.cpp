#include "CGIHandler.hpp"
#include "Webserv.hpp"

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
    std::cout << buffer << std::endl;

    // Get server config
    Server& server = get_server_from_client(socket_fd);
    Config& config = server.config();

    // Parsing Request + rest read buffer
    Request req = Request(buffer);
    std::memset(buffer, 0, BUFFER_SIZE);

    // Start to build the Response { content; content_type; code }
    struct Response res = {"", "", 400};

    // CGI request
    if (!req["URI"].empty() &&
        req["URI"].find(".php", req["URI"].size() - 4) != std::string::npos)
    {
        res.content = handle_cgi(config, req);
        res.code = 200;
    }
    if (!req["URI"].empty() &&
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
        file_to_string("html/400.html", res.content);

    // Send the response in a struct with headers infos
    respond(socket_fd, req, res);
}

std::string Webserv::handle_cgi(Config const& config, Request const& request)
{
    // Just a CGI test here, need more verifications. For exemple if we are in a
    // location
    CGIHandler handler(config, request);

    // To do: get Content-type

    // Isolate body from CGI response
    std::string string_buffer(buffer);
    int         pos = string_buffer.find("\r\n\r\n");
    string_buffer.erase(0, pos + 3);

    handler.execute(buffer);

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
            if (file_to_string((root + "/" + uri).c_str(), content))
            {
                res.code = 200;
                return content;
            }
        }
    }
    if (file_to_string((root + "/" + uri).c_str(), content)) // 200 OK
        res.code = 200;
    else // 404 Not Found
    {
        file_to_string("html/404.html", content);
        res.code = 404;
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
