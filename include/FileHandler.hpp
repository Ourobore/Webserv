#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <cstdio>
#include <iostream>
#include <string>

static int BUF_SIZE = 1024;
// Change to BUFFER_SIZE, but already one in Webserv object

/*
Used to open a file, or an already opened file descriptor, and read data from
it. Extracts file descriptor from stream to use it in poll(), or stream to read
data stream from a file descriptor. Destination file descriptor can be defined
if necessary to specifiy to which file descriptor the data must be sent to.
Returns a std::string.
*/

// For now only handles reading. May add writing option later if necessary

class FileHandler
{
  private:
    FILE* _stream;
    int   _fd;
    int   _dest_fd;
    char* _buffer;

    FileHandler();

  public:
    // Constructors and destructors
    FileHandler(std::string filename);
    FileHandler(int file_descriptor);
    ~FileHandler();

    // Reading
    std::string read_all();

    // Accessors
    FILE* stream();
    int   fd() const;
    int   dest_fd() const;
    char* buffer();
};

#endif