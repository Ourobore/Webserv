#include "Webserv.hpp"

namespace multipart
{
    /* Get multipart/form-data boundary */
    static std::string get_boundary(std::string content_type)
    {
        return (content_type.substr(content_type.find("boundary=") + 9));
    }

    /* Remove body from content and return file name */
    static std::string get_filename(std::string file_content)
    {
        std::string filename =
            file_content.substr(file_content.find("filename=\"") + 10);
        filename = filename.substr(0, filename.find("\""));

        return (filename);
    }

    /* Remove multipart header */
    static void remove_header(std::string& file_content)
    {
        file_content = file_content.substr(file_content.find("\r\r") + 2);
    }
} // namespace multipart

void Webserv::handle_post(Config& config, Request& request,
                          ClientHandler& client)
{
    std::string request_body = request["Body"];
    std::string boundary =
        "--" + multipart::get_boundary(request["Content-Type"]);

    while (request_body != boundary + "--\r") // End of body, with end boundary
    {
        if (request_body.find(boundary) == 0) // Update if boundary is first
        {
            request_body = request_body.substr(boundary.length() + 1);
            continue;
        }

        // Get file content: header + file content
        std::string file_content =
            request_body.substr(0, request_body.find(boundary));
        int file_length = file_content.length();

        // Get filename and remove header from file_content
        std::string filename = multipart::get_filename(file_content);
        multipart::remove_header(file_content);

        // Opening file and storing content
        FileHandler new_file = ft::open_file_stream(
            "requirements/upload/" + filename, config, "w+");
        new_file.set_string_output(file_content);

        // Remove part we just parsed
        request_body = request_body.substr(file_length);

        // Updating pfds and files vector
        struct pollfd pfd = {new_file.fd(), POLLOUT, 0};
        pfds.push_back(pfd);
        client.files().push_back(new_file);
    }
}
