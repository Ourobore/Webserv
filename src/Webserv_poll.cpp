#include "Webserv.hpp"
#include <sys/socket.h>

void Webserv::poll_events()
{
    for (size_t i = 0; i < pfds.size(); i++)
    {
        ClientHandler* client = get_client(pfds[i].fd);

        if (is_file_fd(pfds[i].fd))
            poll_file(*client, i);
        else
        {
            /* If there is nothing more to read we process the request */
            if (pfds[i].revents == 0 && client &&
                (request_ready(*client, *client->request()) ||
                 request_not_treated(*client)))
                request_handler(*client, get_server_from_client(
                                             client->fd(),
                                             client->request()->tokens["Host"])
                                             .config());

            // Check if someone has something to read
            if (pfds[i].revents & POLLIN)
            {
                // If this is a server, handle new connection
                if (is_server_socket(pfds[i].fd))
                    accept_connection(pfds[i].fd);
                // Or if this is a client, read the data available
                else
                    recv_chunk(*client, i);
            }
            /* Else if fd is ready to write, write response, then set events to
               POLLIN again to be ready to read next time */
            else if (pfds[i].revents & POLLOUT)
                response_handler(*client, i);
        }
    }
}

/* Read the data that is available on the socket, to a limit of READ_SIZE
   bytes. Append what is read to a request buffer, or directly to the request.
   Disconnect the client if no data is available */
void Webserv::recv_chunk(ClientHandler& client, int client_index)
{
    int  recv_ret = 0;
    char chunk[READ_SIZE + 1] = {0};

    recv_ret = recv(pfds[client_index].fd, chunk, READ_SIZE, 0);
    if ((recv_ret == 0 && client.raw_request.length() == 0) || recv_ret == -1)
        close_connection(recv_ret, client_index);
    else
    {
        // If we haven't parsed the request yet
        if (!client.request())
        {
            client.raw_request.append(chunk, recv_ret);

            // If we have everything to parse the request
            if (client.raw_request.find("\r\n\r\n") != std::string::npos)
            {
                client.set_request(
                    get_server_from_client(client.fd(), client.raw_request)
                        .config());
            }
        }
        // If request already parsed, then everything else is body
        else
        {
            if (client.request()->tokens["Transfer-Encoding"].find("chunked") !=
                std::string::npos)
                parse_chunk(client, chunk, recv_ret);
            else
            {
                client.request()->tokens["Body"].append(chunk, recv_ret);
                ft::add_content_length(
                    client.request()->tokens["Content-Length"], recv_ret);
            }
        }
    }
}

void Webserv::parse_chunk(ClientHandler& client, char* raw_chunk, int recv_ret)
{
    client.raw_request.append(raw_chunk, recv_ret);
    Chunk* chunk = client.request()->chunk();

    if (chunk)
    {
        chunk->append(client.raw_request);
        if (chunk->completed())
        {
            client.request()->tokens["Body"].append(chunk->chunk());
            delete chunk;
        }
    }
    while (Chunk::creation_possible(client.raw_request))
    {
        chunk = new Chunk(client.raw_request);
        if (chunk->completed())
        {
            client.request()->tokens["Body"].append(chunk->chunk());
            delete chunk;
        }
        else
            client.request()->set_chunk(chunk);
    }
    if (Chunk::empty_chunk(client.raw_request))
        client.request()->all_chunks_received = true;
}

bool Webserv::request_ready(ClientHandler& client, Request& request)
{
    return (client.request() &&
            ((request.tokens["Transfer-Encoding"].find("chunked") ==
                  std::string::npos &&
              client.files().size() == 0) ||
             (request.tokens["Transfer-Encoding"].find("chunked") !=
                  std::string::npos &&
              client.files().size() == 0 && request.all_chunks_received)));
}

bool Webserv::request_not_treated(ClientHandler& client)
{
    if (!client.request() && !client.raw_request.empty())
    {
        client.set_request(
            get_server_from_client(client.fd(), client.raw_request).config());
        return (true);
    }
    else
        return (false);
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

/* If the file descriptor corresponds to a file, return an iterator to the
   FileHandler equivalent to this file. If not, return an end() iterator */
FileHandler* Webserv::is_file_fd(int file_descriptor)
{
    std::vector<ClientHandler>::iterator client_it;

    for (client_it = clients.begin(); client_it != clients.end(); ++client_it)
    {
        std::vector<FileHandler>::iterator file_it;
        for (file_it = client_it->files().begin();
             file_it != client_it->files().end(); ++file_it)
            if (file_it->fd() == file_descriptor)
                return (&(*file_it));
    }
    return (NULL);
}

bool Webserv::is_cgi_input(ClientHandler& client, int file_descriptor)
{
    if (client.cgi() && client.cgi()->input_pipe &&
        file_descriptor == client.cgi()->input_pipe[PIPEWRITE])
        return (true);
    else
        return (false);
}

/* Get the index that corresponds to file descriptor in the pollfd structure
 */
int Webserv::get_poll_index(int file_descriptor)
{
    for (size_t i = 0; i < pfds.size(); ++i)
        if (pfds[i].fd == file_descriptor)
            return (i);
    return (-1);
}
