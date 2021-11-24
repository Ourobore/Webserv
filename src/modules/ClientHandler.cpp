#include "ClientHandler.hpp"
#include "CGIHandler.hpp"

// Constructor and destructor
ClientHandler::ClientHandler(int client_fd) : _fd(client_fd), _cgi(NULL)
{
    this->_request = NULL;
    this->raw_request = "";  // To modify tmp
    this->request_bytes = 0; // To modify tmp
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

Request* ClientHandler::request()
{
    return (_request);
}

std::vector<FileHandler>& ClientHandler::files()
{
    return (_files);
}

ClientHandler::Response& ClientHandler::response()
{
    return (_response);
}

CGIHandler* ClientHandler::cgi()
{
    return (_cgi);
}

void ClientHandler::clear_response()
{
    _response.content = "";
    _response.content_type = "";
    _response.date = "";
    _response.location = "";
    _response.code = 0;
}

void ClientHandler::clear_request()
{
    delete _request;
    _request = NULL;
}

void ClientHandler::set_request(Config& config)
{
    _request = new Request(this->raw_request, config);
}

void ClientHandler::set_cgi(CGIHandler* cgi)
{
    _cgi = cgi;
}

void ClientHandler::set_content_type(std::string uri_path,
                                     Config&     server_config)
{
    std::string file_extension =
        uri_path.substr(uri_path.find_last_of('.') + 1);
    _response.content_type = server_config.get_mimetypes()[file_extension];
}

void ClientHandler::set_location_header(std::string uri_path)
{
    _response.location = uri_path;
}

void ClientHandler::set_date()
{
    struct timeval tv;
    time_t         nowtime;
    struct tm*     nowtm;
    char           tmbuf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = gmtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);

    _response.date = std::string(tmbuf);
}