#include "Webserv.hpp"

/* Get the corresponding file, read it all, and then close it and remove it from
   the client pending files and poll file descriptor array. Lastly, set the
   corresponding client events to POLLOUT to write response when possible */
void Webserv::poll_file(ClientHandler& client, size_t& file_index)
{
    FileHandler* file = is_file_fd(pfds[file_index].fd);

    if (pfds[file_index].revents & POLLIN)
    {
        if (client.cgi() && client.cgi()->output_pipe)
            close(client.cgi()->output_pipe[PIPEWRITE]);

        try
        {
            bool eof = file->read_chunk();
            if (eof)
            {
                // Set content and status response
                client.response().content = file->string_output();
                client.response().code = file->status();

                // Setting up client to POLLOUT and remove file fd from pfds
                pfds[get_poll_index(client.fd())].events = POLLOUT;
                pfds.erase(pfds.begin() + file_index);

                // FileHandler cleaning
                if (file->stream())
                    fclose(file->stream());
                client.files().erase(client.files().begin());

                // If CGI, remove CGI header, then delete ClientHandler::_cgi
                if (client.cgi())
                {
                    // Get Content-Type and remove CGI Header
                    std::string& content = client.response().content;
                    std::string& content_type = client.response().content_type;
                    content_type =
                        content.substr(content.find("Content-type: ") + 14);
                    content_type =
                        content_type.substr(0, content_type.find(";"));
                    int pos = content.find("\r\n\r\n");
                    client.response().content.erase(0, pos + 4);

                    delete client.cgi();
                    client.set_cgi(NULL);
                }
            }
        }
        catch (FileHandler::ReadError)
        {
            pfds.erase(pfds.begin() + file_index);
            fclose(file->stream());
            client.files().erase(client.files().begin());
            Config& config = get_server_from_client(
                                 client.fd(), client.request()->tokens["Host"])
                                 .config();
            wrapper_open_error(client, config, 500);
        }
    }
    else if (pfds[file_index].revents & POLLOUT)
    {
        // If CGI input, write body in input pipe for POST
        if (file->string_output().empty() && is_cgi_input(client, file->fd()))
            file->set_string_output(client.request()->tokens["Body"]);

        // Write data to file / input pipe (CGI)
        int bytes_written = write(file->fd(), file->string_output().c_str(),
                                  file->string_output().length());

        // If error, close FileHandler and open a new one with an error 500
        if (bytes_written < 0)
        {
            fclose(file->stream());
            client.files().erase(client.files().begin());
            pfds.erase(pfds.begin() + file_index);
            --file_index;
            Config& config = get_server_from_client(
                                 client.fd(), client.request()->tokens["Host"])
                                 .config();
            wrapper_open_error(client, config, 500);
            return;
        }

        // If everything is written, if CGI launch it, and clean
        size_t bytes = bytes_written;
        if (bytes == file->string_output().length())
        {
            if (is_cgi_input(client, file->fd()))
            {
                close(file->fd());
                client.cgi()->launch_cgi();
            }

            // FileHandler cleaning
            fclose(file->stream());
            client.files().erase(client.files().begin());
            pfds.erase(pfds.begin() + file_index);
            --file_index;
        }
        else // Else update what there is left to write
            file->set_string_output(file->string_output().substr(bytes));
    }
}