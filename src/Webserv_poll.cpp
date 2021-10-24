#include "Webserv.hpp"
#include <sys/socket.h>

/* Get the corresponding file, read it all, and then close it and remove it from
   the client pending files and poll file descriptor array. Lastly, set the
   corresponding client events to POLLOUT to write response when possible */
void Webserv::poll_file(ClientHandler& client, int poll_index)
{
    FileHandler* file = is_file_fd(pfds[poll_index].fd);

    if (pfds[poll_index].revents & POLLIN)
    {
        file->read_all();
        pfds[get_poll_index(client.fd())].events = POLLOUT;
        pfds.erase(pfds.begin() + poll_index);
        fclose(file->stream());
    }
    // if (pfds[i].revents & POLLOUT) ?
}

/* Write response to server, then erase the request and the file associated from
   the client. Lastly sets the client events bit mask back to POLLIN to be ready
   to read the next request */
void Webserv::poll_response(ClientHandler& client, int poll_index)
{
    response_handler(client);
    client.requests().erase(client.requests().begin());
    client.files().erase(client.files().begin());
    pfds[poll_index].events = POLLIN;
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
                poll_response(client, i);
        }
    }
}

/* Read all the data on the file descriptor, and append it in recv_output.
   Does it need MSG_DONTWAIT as flags? Same does it need binary reading?*/
int Webserv::recv_all(int file_descriptor, std::string& recv_output, int flags)
{
    int  recv_ret = 0;
    int  bytes_received = 0;
    char chunk[CHUNK_SIZE] = {0};

    while ((recv_ret = recv(file_descriptor, chunk, CHUNK_SIZE - 1, flags)) > 0)
    {
        recv_output += std::string(chunk);
        bytes_received += recv_ret;
        memset(chunk, 0, CHUNK_SIZE);
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
