#include "FileHandler.hpp"
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cstring>
#include <string>

// Constructors and destructors
FileHandler::FileHandler()
{
    _stream = NULL;
    _fd = -1;
    _dest_fd = -1;
    _buffer = NULL;
    _status = -1;
}

FileHandler::FileHandler(std::string filename, std::string mode)
{
    // Opening file stream
    if (!(_stream = fopen(filename.c_str(), mode.c_str())))
    {
        if (errno == ENOENT)
            throw FileHandler::NoFile();
        else
            throw FileHandler::OpenError();
    }

    // Getting file descriptor for poll()
    _fd = fileno(_stream);

    // Initializing file dedicated buffer
    _buffer = new char[BUF_SIZE + 1]();
}

FileHandler::FileHandler(int file_descriptor, std::string mode)
    : _fd(file_descriptor)
{
    // Getting file stream from file descriptor
    if (!(_stream = fdopen(file_descriptor, mode.c_str())))
    {
        if (errno == ENOENT)
            throw FileHandler::NoFile();
        else
            throw FileHandler::OpenError();
    }

    // Initializing file dedicated buffer
    _buffer = new char[BUF_SIZE + 1]();
}

FileHandler::~FileHandler()
{
    if (_stream)
        fclose(_stream);
    if (_buffer)
        delete[] _buffer;
}

// Reading
std::string FileHandler::read_all()
{
    std::string string_buffer("");

    // Perhaps there is a better syntax, but man page says to catch EOF and
    // errors with functions, not return values
    while (fread(_buffer, sizeof(char), BUF_SIZE, _stream))
    {
        // If there is a read error, throw error
        if (ferror(_stream))
            throw FileHandler::ReadError();

        // Concatenate read buffer with total output
        string_buffer += _buffer;
        std::memset(_buffer, 0, BUF_SIZE + 1);

        // If it is EOF, then break
        if (feof(_stream))
            break;
    }
    return (string_buffer);
}

int FileHandler::read_all(std::string& string_buffer)
{
    // Perhaps there is a better syntax, but man page says to catch EOF and
    // errors with functions, not return values
    while (fread(_buffer, sizeof(char), BUF_SIZE, _stream))
    {
        // If there is a read error, return error
        if (ferror(_stream))
            return (0);

        // Concatenate read buffer with total output
        string_buffer += _buffer;
        std::memset(_buffer, 0, BUF_SIZE + 1);

        // If it is EOF, then break
        if (feof(_stream))
            break;
    }
    return (1);
}

// Accessors
FILE* FileHandler::stream()
{
    return (_stream);
}

int FileHandler::fd() const
{
    return (_fd);
}

int FileHandler::dest_fd() const
{
    return (_dest_fd);
}

char* FileHandler::buffer()
{
    return (_buffer);
}

int FileHandler::status() const
{
    return (_status);
}

void FileHandler::set_status(int status)
{
    _status = status;
}