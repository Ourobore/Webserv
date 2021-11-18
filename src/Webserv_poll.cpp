#include "Webserv.hpp"
#include <sys/socket.h>

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
        if (file->stream())
            file->read_all();

        // Set content and status response
        client.response().content = file->string_output();
        client.response().code = file->status();

        // Setting up client to POLLOUT and remove file fd from pfds
        pfds[get_poll_index(client.fd())].events = POLLOUT;
        pfds.erase(pfds.begin() + file_index);
        //--file_index; // Faster ? No cause crash. Why? does the other one too?

        // FileHandler cleaning
        if (file->stream())
            fclose(file->stream());
        client.files().erase(client.files().begin());

        // If CGI, clean output from CGI header, then delete ClientHandler::_cgi
        if (client.cgi())
        {

            int pos = client.response().content.find("\r\n\r\n");
            client.response().content.erase(0, pos + 4);

            delete client.cgi();
            client.set_cgi(NULL);
        }
    }
    else if (pfds[file_index].revents & POLLOUT)
    {
        // Write file content from upload
        write(file->fd(), file->string_output().c_str(),
              file->string_output().length());

        // FileHandler cleaning
        fclose(file->stream());
        client.files().erase(client.files().begin());
        pfds.erase(pfds.begin() + file_index);
        --file_index;
    }
}

void Webserv::poll_events()
{
    for (size_t i = 0; i < pfds.size(); i++)
    {
        ClientHandler& client = get_client(pfds[i].fd);

        if (is_file_fd(pfds[i].fd))
            poll_file(client, i);
        else
        {
            // Check if someone has something to read
            if (pfds[i].revents & POLLIN)
            {
                // If this is a server, handle new connection
                if (is_server_socket(pfds[i].fd))
                    accept_connection(pfds[i].fd);
                // Or if this is a client
                else
                {
                    // DEBUG: check recv loop condition
                    recv_data = "";
                    int bytes_received =
                        recv_all(pfds[i].fd, recv_data, MSG_DONTWAIT);

                    if (bytes_received <= 0)
                        close_connection(bytes_received, i);
                    else
                        request_handler(
                            client,
                            get_server_from_client(client.fd()).config());
                }
            }
            /* Else if fd is ready to write, write response, then set events to
               POLLIN again to be ready to read next time */
            else if (pfds[i].revents & POLLOUT)
                response_handler(client, i);
        }
    }
}

/* Read all the data on the file descriptor, and append it in recv_output.
   Does it need MSG_DONTWAIT as flags? Same does it need binary reading?*/
int Webserv::recv_all(int file_descriptor, std::string& recv_output, int flags)
{
    int  recv_ret = 0;
    int  bytes_received = 0;
    char chunk[CHUNK_SIZE + 1] = {0};

    while ((recv_ret = recv(file_descriptor, chunk, CHUNK_SIZE, flags)) > 0)
    {
        recv_output.append(chunk, recv_ret);
        bytes_received += recv_ret;
        memset(chunk, 0, CHUNK_SIZE + 1);
    }
    return (bytes_received);
}

/* Check if the file descriptor corresponds to a server,
    a.k.a is in the the vector Webserv::servers object*/
bool Webserv::is_server_socket(int socket_fd)
{
    std::vector<Server>::iterator it;

    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->sockfd() == socket_fd)
            return (true);
    }
    return (false);
}

/* Get the index that corresponds to file descriptor in the pollfd structure */
int Webserv::get_poll_index(int file_descriptor)
{
    for (size_t i = 0; i < pfds.size(); ++i)
        if (pfds[i].fd == file_descriptor)
            return (i);
    return (-1);
}
