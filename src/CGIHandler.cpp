#include "CGIHandler.hpp"
#include "Config.hpp"
#include "Request.hpp"

CGIHandler::CGIHandler(Config config, Request request)
{
    (void)config;
    (void)request;
    variables["GATEWAY_INTERFACE"] = "CGI/1.1";
    variables["SERVER_PROTOCOL"] = "HTTP/1.1";
    variables["SERVER_SOFTWARE"] = "Webserv";
}

CGIHandler::~CGIHandler()
{
}