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
        std::string filename("");

        if (!file_content.empty())
        {
            filename =
                file_content.substr(file_content.find("filename=\"") + 10);
            filename = filename.substr(0, filename.find("\""));
        }

        return (filename);
    }

    /* Remove multipart header */
    static void remove_header(std::string& file_content)
    {
        file_content = file_content.substr(file_content.find("\r\n\r\n") + 4);
    }
} // namespace multipart

void Webserv::handle_upload(Config& config, Request& request,
                            ClientHandler& client)
{
    std::string request_body = request["Body"];
    std::string boundary =
        "--" + multipart::get_boundary(request["Content-Type"]);

    while (request_body + "\r" !=
           boundary + "--\r") // End of body, with end boundary
    {
        if (request_body.find(boundary + "\r") == 0) // If we hit boundary
        {
            request_body = request_body.substr(boundary.length() + 1);
            continue;
        }

        // Get file content: multipart header + file content
        std::string file_content =
            request_body.substr(0, request_body.find(boundary));
        int file_length = file_content.length();

        // Get filename and remove header from file_content
        std::string filename = multipart::get_filename(file_content);
        if (!filename.empty())
        {
            multipart::remove_header(file_content);

            // Opening file and storing content
            FileHandler new_file = ft::open_file_stream(
                "requirements/upload/" + filename, config, "w+");
            new_file.set_string_output(file_content);

            // Updating pfds and files vector
            struct pollfd pfd = {new_file.fd(), POLLOUT, 0};
            pfds.push_back(pfd);
            client.files().push_back(new_file);
        }

        // Remove part we just parsed
        request_body = request_body.substr(file_length);
    }
}
