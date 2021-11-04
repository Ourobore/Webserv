#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include "FileHandler.hpp"
#include "Request.hpp"

class CGIHandler;

class ClientHandler
{
  public:
    typedef struct Response
    {
        std::string content;
        std::string content_type;
        int         code;
    } Response;

  private:
    int                      _fd;
    std::vector<Request>     _requests;
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
    std::vector<Request>&     requests();
    std::vector<FileHandler>& files();
    Response&                 response();
    CGIHandler*               cgi();
    void                      set_cgi(CGIHandler* cgi);
};

#endif