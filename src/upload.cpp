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

    /* Wrapper that loop on all request body to extract every file content */
    static void get_files(Config& config, Request& request,
                          ClientHandler& client, std::vector<pollfd>& pfds)
    {
        Location    location = config.get_locations()[request.location_index()];
        std::string request_body = request["Body"];
        std::string boundary =
            "--" + multipart::get_boundary(request["Content-Type"]);

        while (request_body !=
               boundary + "--\r\n") // End of body, with end boundary
        {
            if (request_body.find(boundary + "\r\n") == 0) // If we hit boundary
            {
                request_body = request_body.substr(boundary.length() + 2);
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
                    location.get_upload() + "/" + filename, config, "w+");
                if (new_file.status() ==
                    200) // Or internal error if one failed?
                {
                    new_file.set_string_output(file_content);

                    // Updating pfds and files vector
                    struct pollfd pfd = {new_file.fd(), POLLOUT, 0};
                    pfds.push_back(pfd);
                    client.files().push_back(new_file);
                }
            }

            // Remove part we just parsed
            request_body = request_body.substr(file_length);
        }
    }
} // namespace multipart

void Webserv::handle_upload(Config& config, Request& request,
                            ClientHandler& client)
{
    Location location = config.get_locations()[request.location_index()];
    if (location.get_upload().empty()) // If upload is not authorized
    {
        generate::response(client, 403);
        pfds[get_poll_index(client.fd())].events = POLLOUT;
    }
    else // Should we check if upload path is correct?
    {
        multipart::get_files(config, request, client, pfds);

        // We need to answer the pending POST request
        client.set_date();
        if (!ft::is_dir(request["URI"]))
        {
            FileHandler file = ft::open_file_stream(request["URI"], config);
            if (file.stream())
            {
                client.set_content_type(request["URI"], config);
                client.files().push_back(file);
                struct pollfd file_poll = {file.fd(), POLLIN, 0};
                pfds.push_back(file_poll);
                return;
            }
            else if (file.status() != 200) // If file opening failed
            {
                client.response().code = file.status();
                client.response().content = "text/html";
                client.response().content = generate::error_page(file.status());
                pfds[get_poll_index(client.fd())].events = POLLOUT;
            }
        }
        // If dir?
    }
}
