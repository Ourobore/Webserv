#include "Clients.hpp"
#include <sys/poll.h>

Clients::Clients(struct pollfd& server_poll) : _size(1), _capacity(1)
{
    poll = new (struct pollfd);
    poll[0].fd = server_poll.fd;
    poll[0].events = server_poll.events;
    poll[0].revents = server_poll.revents;
}

Clients::~Clients()
{
    delete poll;
    _size = 0;
    _capacity = 0;
}

void Clients::reallocate()
{
    struct pollfd* new_pfds;

    new_pfds = reinterpret_cast<pollfd*>(
        malloc(sizeof(struct pollfd) * _capacity * 2));

    for (int i = 0; i < _size; ++i)
    {
        new_pfds[i].fd = poll[i].fd;
        new_pfds[i].events = poll[i].events;
        new_pfds[i].revents = poll[i].revents;
    }
    free(poll);
    _capacity *= 2;
    poll = new_pfds;
}

void Clients::check_error(int value, const std::string message)
{
    if (value < 0)
    {
        std::cerr << "Error: " << message << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Clients::add_client(int client_socket_fd)
{
    if (_size == _capacity)
        reallocate();

    poll[_size].fd = client_socket_fd;
    poll[_size].events = POLLIN;
    ++_size;
}

// Can be optimized with only moving the last client in client_index
void Clients::remove_client(int client_index)
{
    poll[client_index].fd = 0;
    poll[client_index].events = 0;
    poll[client_index].revents = 0;
    for (int i = client_index; i < _size - 1; ++i)
    {
        poll[i].fd = poll[i + 1].fd;
        poll[i].events = poll[i + 1].events;
        poll[i].events = poll[i + 1].events;
    }
    poll[_size - 1].fd = 0;
    poll[_size - 1].events = 0;
    poll[_size - 1].events = 0;
    --_size;
}

struct pollfd* Clients::get_poll()
{
    return (poll);
}

int Clients::size() const
{
    return (_size);
}

int Clients::capacity() const
{
    return (_capacity);
}
