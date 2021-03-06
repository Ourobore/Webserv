#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

static int BUF_SIZE = 65535; // 2^16 - 1

/*
Used to open a file, or an already opened file descriptor, and read data from
it. Extracts file descriptor from stream to use it in poll(), or stream to read
data from a file descriptor. Destination file descriptor can be defined
if necessary to specifiy to which file descriptor the data must be sent to.
Returns a std::string.
*/

// For now only handles reading. May add writing option later if necessary

class FileHandler
{
  private:
    FILE*       _stream;
    int         _fd;
    int         _status;
    std::string _string_output;

  public:
    // Constructors and destructor
    FileHandler();
    FileHandler(std::string filename, std::string mode = "r"); // Throws
    FileHandler(int file_descriptor, std::string mode = "r");  // Throws
    ~FileHandler();

    // Reading
    std::string read_all(); // Throws
    int         read_all(std::string& string_buffer);
    bool        read_chunk();

    // Accessors
    FILE*       stream();
    int         fd() const;
    int         status() const;
    std::string string_output() const;
    void        set_string_output(std::string string_output);
    void        set_status(int status);

    // Exceptions
    class OpenError : public std::exception
    {
      public:
        virtual const char* what() const throw()
        {
            return ("Could not open file");
        }
    };

    class ReadError : public std::exception
    {
      public:
        virtual const char* what() const throw()
        {
            return ("File reading error");
        }
    };

    class NoFile : public std::exception
    {
      public:
        virtual const char* what() const throw()
        {
            return ("File doesn't exists");
        }
    };

    class IsDir : public std::exception
    {
      public:
        virtual const char* what() const throw()
        {
            return ("Cannot write on directory");
        }
    };

    // Panic button
    void clean_all();
};

#endif