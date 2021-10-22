#include "FileHandler.hpp"
#include <cerrno>
#include <cstring>

// Constructors and destructors
FileHandler::FileHandler()
{
    _stream = NULL;
    _fd = -1;
    _dest_fd = -1;
    _status = -1;
    _string_output = "";
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
}

FileHandler::~FileHandler()
{
    if (_stream)
        fclose(_stream);
}

// Reading
std::string FileHandler::read_all()
{
    size_t nb_elem_read = 0;
    char   buffer[BUF_SIZE + 1];

    // Perhaps there is a better syntax, but man page says to
    // catch EOF and errors with functions, not return values
    while ((nb_elem_read = fread(buffer, sizeof(char), BUF_SIZE, _stream)))
    {
        // If there is a read error, throw error
        if (ferror(_stream))
            throw FileHandler::ReadError();

        // Concatenate read buffer with total output
        _string_output.append(buffer, nb_elem_read);
        std::memset(buffer, 0, BUF_SIZE + 1);

        // If it is EOF, then break
        if (feof(_stream))
            break;
    }
    return (_string_output);
}

int FileHandler::read_all(std::string& string_buffer)
{
    // Perhaps there is a better syntax, but man page says to catch EOF and
    // errors with functions, not return values
    size_t nb_elem_read = 0;
    char   buffer[BUF_SIZE + 1];

    while ((nb_elem_read = fread(buffer, sizeof(char), BUF_SIZE, _stream)))
    {
        // If there is a read error, return error
        if (ferror(_stream))
            return (0);

        // Concatenate read buffer with total output
        string_buffer.append(buffer, nb_elem_read);
        std::memset(buffer, 0, BUF_SIZE + 1);

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

int FileHandler::status() const
{
    return (_status);
}

std::string FileHandler::string_output() const
{
    return (_string_output);
}

void FileHandler::set_status(int status)
{
    _status = status;
}