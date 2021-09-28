#include "Webserv.hpp"

// Handle clients requests
void Webserv::handle(int socket_index)
{
    // Print + parsing new Request + clear buffer for next request
    std::cout << buffer << std::endl;
    Request req = Request(buffer);
    std::memset(buffer, 0, BUFFER_SIZE);

    // Start to build the response
    std::string content = "<h1>Hello from Webserv !</h1>\r\n";
    std::string uri;

    // handle root uri
    if (req.tokens.find("URI") != req.tokens.end())
    {
        if (req.tokens.find("URI")->second == "/")
            uri = "/index";
        else
            uri = req.tokens.find("URI")->second;

        // Open resource from URI in html/ directory and convert to string
        std::ifstream     ifs(("html" + uri + ".html").c_str());
        std::stringstream buf;
        if (ifs.is_open())
        {
            buf << ifs.rdbuf();
            content = buf.str();
        }
    }
    respond(socket_index, content);
}

void Webserv::respond(int i, std::string content)
{

    // Response headers for web browser clients
    std::stringstream headers_content;
    headers_content << "HTTP/1.1 200 OK\r\n"
                    << "Connection: keep-alive\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << content.length() << "\r\n"
                    << "\r\n";
    std::string headers = headers_content.str();

    // send to the client through his socket
    send(pfds[i].fd, headers.c_str(), headers.length(), 0);
    send(pfds[i].fd, content.c_str(), content.length(), 0);
}