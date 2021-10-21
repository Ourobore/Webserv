#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include "FileHandler.hpp"
#include "Request.hpp"

class ClientHandler
{
  private:
    int                      _fd;
    std::vector<Request>     _requests;
    std::vector<FileHandler> _files;

    ClientHandler();

  public:
    // Constructor and destructor
    ClientHandler(int client_fd);
    ~ClientHandler();

    // Accessors
    int                       fd() const;
    std::vector<Request>&     requests();
    std::vector<FileHandler>& files();
};

#endif