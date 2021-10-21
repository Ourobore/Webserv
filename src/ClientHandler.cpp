#include "ClientHandler.hpp"

// Constructor and destructor
ClientHandler::ClientHandler(int client_fd) : _fd(client_fd)
{
}

ClientHandler::~ClientHandler()
{
}

// Accessors
int ClientHandler::fd() const
{
    return (_fd);
}

std::vector<Request>& ClientHandler::requests()
{
    return (_requests);
}

std::vector<FileHandler>& ClientHandler::files()
{
    return (_files);
}