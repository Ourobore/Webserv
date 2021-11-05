#include "ClientHandler.hpp"
#include "CGIHandler.hpp"

// Constructor and destructor
ClientHandler::ClientHandler(int client_fd) : _fd(client_fd), _cgi(NULL)
{
}

ClientHandler::~ClientHandler()
{
}

// Operator overload
bool ClientHandler::operator==(ClientHandler const& rhs) const
{
    return (_fd == rhs.fd());
}

bool ClientHandler::operator!=(ClientHandler const& rhs) const
{
    return !(*this == rhs);
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

ClientHandler::Response& ClientHandler::response()
{
    return (_response);
}

void ClientHandler::set_content_type(std::string uri_path,
                                     Config&     server_config)
{
    std::string file_extension =
        uri_path.substr(uri_path.find_last_of('.') + 1);
    _response.content_type = server_config.get_mimetypes()[file_extension];
}
CGIHandler* ClientHandler::cgi()
{
    return (_cgi);
}

void ClientHandler::set_cgi(CGIHandler* cgi)
{
    _cgi = cgi;
}

std::string ClientHandler::set_date()
{
    struct timeval tv;
    time_t         nowtime;
    struct tm*     nowtm;
    char           tmbuf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);
    return std::string(tmbuf);
}