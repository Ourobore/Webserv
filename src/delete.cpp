#include "Webserv.hpp"

void Webserv::handle_delete(Config& server_config, Request& request,
                            ClientHandler& client)
{
    (void)server_config;

    int status = remove(request.tokens["URI"].c_str());
    if (status == -1)
        status = 202; // ACCEPTED
    else
    {
        status = 200; // NO CONTENT
        /* Or status = 204 if we don't write a message in body */
        client.response().content = generate::file_deleted();
        client.response().content_type = "text/html";
    }

    client.response().code = status;
    client.set_date();
    ClientHandler::Response response = client.response();

    // Set client to POLLOUT to write response
    pfds[get_poll_index(client.fd())].events = POLLOUT;
}