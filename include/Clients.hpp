#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include "Socket.hpp"
#include <sys/poll.h>

class Clients
{
  private:
    struct pollfd* poll;
    int            _size;
    int            _capacity;

    void reallocate();

  public:
    Clients();
    ~Clients();

    static void check_error(int value, const std::string message);
    void        add_client(int client_socket_fd);
    void        remove_client(int client_index);

    struct pollfd* get_poll();
    int            size() const;
    int            capacity() const;
};

#endif