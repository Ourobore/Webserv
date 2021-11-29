#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include "FileHandler.hpp"
#include "Request.hpp"

#include <sys/time.h>

class CGIHandler;

class ClientHandler
{
  public:
    typedef struct Response
    {
        std::string content;
        std::string content_type;
        std::string date;
        std::string location;
        int         code;
        bool        chunked;
    } Response;

  private:
    int                      _fd;
    Request*                 _request;
    std::vector<FileHandler> _files;
    Response                 _response;
    CGIHandler*              _cgi;

    ClientHandler();

  public:
    // Constructor and destructor
    ClientHandler(int client_fd);
    ~ClientHandler();

    // Operator overloads
    bool operator==(ClientHandler const& rhs) const;
    bool operator!=(ClientHandler const& rhs) const;

    // Accessors
    int                       fd() const;
    Request*                  request();
    std::vector<FileHandler>& files();
    Response&                 response();
    CGIHandler*               cgi();

    void clear_response();
    void clear_request();
    void set_request(Config& config);
    void set_content_type(std::string uri_path, Config& server_config);
    void set_date();
    void set_cgi(CGIHandler* cgi);
    void set_location_header(std::string uri_path);

    // Read string buffer
    std::string raw_request;
};

#endif