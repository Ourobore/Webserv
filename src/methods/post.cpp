#include "Webserv.hpp"

void Webserv::handle_post(Config& config, Request& request,
                          ClientHandler& client)
{
    if (CGIHandler::is_cgi_file(request["URI"], request.location_index(),
                                config))
    {
        CGIHandler* handler = new CGIHandler(config, request, client.fd());
        handler->setup_cgi(client, pfds, config);
    }
    else
        wrapper_open_file(client, config, request);
}