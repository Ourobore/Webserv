#include "Webserv.hpp"

/* Remove body from content and return file name */
static std::string clean_content(std::string& file_content)
{
    std::string filename =
        file_content.substr(file_content.find("filename=\"") + 10);
    filename = filename.substr(0, filename.find("\""));

    file_content = file_content.substr(file_content.find("\r\r") + 2);
    return (filename);
}

void Webserv::handle_post(Config& config, Request& request,
                          ClientHandler& client)
{
    std::string request_body = request["Body"];
    std::string boundary =
        "--" + request["Content-Type"].substr(
                   request["Content-Type"].find("boundary=") + 9);

    (void)client;
    (void)config;
    while (request_body != boundary + "--") // End of body, with ending boundary
    {
        if (request_body.find(boundary) == 0) // Update if boundary is first
        {
            request_body = request_body.substr(boundary.length() + 1);
            continue;
        }
        std::string file_body =
            request_body.substr(0, request_body.find(boundary));

        std::string filename = clean_content(request_body);
        std::cout << "filename: " << filename << std::endl
                  << "content: " << request_body << std::endl;
        exit(1);
        // FileHandler new_file;
    }
}
