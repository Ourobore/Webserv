#include "Webserv.hpp"

void Webserv::handle_delete(Request& request, ClientHandler& client)
{
    int status = remove(request.tokens["URI"].c_str());
    if (status == -1)
        status = 202; // ACCEPTED
    else
    {
        status = 200; // OK
        /* Or status = 204 'NO CONTENT' if we don't write a message in body */
        client.response().content = generate::file_deleted();
        client.response().content_type = "text/html";
    }
    client.response().code = status;
    client.set_date();

    // Set client to POLLOUT to write response
    pfds[get_poll_index(client.fd())].events = POLLOUT;
}