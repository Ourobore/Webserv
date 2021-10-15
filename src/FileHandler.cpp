#include "FileHandler.hpp"

// Constructors and destructors
FileHandler::FileHandler(std::string filename)
{
    // Opening file stream
    if (!(_stream = fopen(filename.c_str(), "r")))
        std::cout << "fopen()" << std::endl; // Must throw error later

    // Getting file descriptor for poll()
    _fd = fileno(_stream);

    // Initializing file dedicated buffer
    _buffer = new char[BUF_SIZE + 1]();
}

FileHandler::FileHandler(int file_descriptor) : _fd(file_descriptor)
{
    // Getting file stream from file descriptor
    if (!(_stream = fdopen(file_descriptor, "r")))
        std::cout << "fdopen()" << std::endl; // Must throw error later

    // Initializing file dedicated buffer
    _buffer = new char[BUF_SIZE + 1]();
}

FileHandler::~FileHandler()
{
    fclose(_stream);
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
        // If there is a read error, break
        if (ferror(_stream))
        {
            std::cout << "fread()" << std::endl; // Must throw error later
            break;
        }

        // Concatenate read buffer with total output
        string_buffer += _buffer;

        // If it is EOF, then break
        if (feof(_stream))
            break;
    }
    return (string_buffer);
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