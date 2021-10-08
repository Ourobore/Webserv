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
    // get server config
    std::cout << buffer << std::endl;

    Server& server = get_server_from_client(socket_fd);
    Config& config = server.config();

    // Debug
    std::cout << "Host: " << config.get_host() << ", Port:" << config.get_port()
              << ", Root:" << config.get_root() << std::endl;

    // Parsing Request
    Request req = Request(buffer);
    std::memset(buffer, 0, BUFFER_SIZE);

    // Start to build the response
    std::string content = "";
    std::string uri = "";
    int         code = 400;

    // CGI request
    if (!req["URI"].empty() &&
        req["URI"].find(".php", req["URI"].size() - 4) != std::string::npos)
    {
        content = handle_cgi(config, req);
        code = 200;
    }
    // Simple resource request is valid
    else if (!req["URI"].empty())
    {
        // TODO: Get root location
        if (req["URI"] == "/")
            uri = "/index.html";
        else
            uri = req["URI"];

        // 200 OK
        if (file_to_string(("html" + uri).c_str(), content))
            code = 200;
        // 404 Not Found
        else
        {
            file_to_string("html/404.html", content);
            code = 404;
        }
    }
    // 400 Bad Request
    else if (req["URI"].empty() || req["Method"].empty())
        file_to_string("html/400.html", content);

    respond(socket_fd, code, content);
}

std::string Webserv::handle_cgi(Config const& config, Request const& request)
{
    // Just a CGI test here, need more verifications. For exemple if we are in a
    // location
    std::cout << "It's a PHP file!" << std::endl; // To remove
    CGIHandler handler(config, request);
    handler.execute(buffer);

    // To do: get Content-type

    // Isolate body from CGI response
    std::string string_buffer(buffer);
    int         pos = string_buffer.find("\r\n\r\n");
    string_buffer.erase(0, pos + 3);

    return (string_buffer);
}

void Webserv::respond(int socket_fd, int code, std::string content)
{
    std::string connection = "close";
    if (code == 200)
        connection = "keep-alive";

    // Response headers for web browser clients
    std::stringstream headers_content;
    headers_content << "HTTP/1.1 " << code << " " << res_status[code] << "\r\n"
                    << "Server: webserv/42\r\n"
                    << "Date: \r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << content.length() << "\r\n"
                    << "Connection: " << connection << "\r\n"
                    << "\r\n";

    std::string headers = headers_content.str();

    // send to the client through his socket
    send(socket_fd, headers.c_str(), headers.length(), 0);
    send(socket_fd, content.c_str(), content.length(), 0);

    // Should we close the connection if status code 400 or 404 ?
    // if (connection == "close")
    //    close_connection(0, i);
}
