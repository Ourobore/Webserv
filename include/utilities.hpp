#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include "Config.hpp"
#include "FileHandler.hpp"
#include "Request.hpp"
#include "generate.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace ft
{
    /* Take a value of type T and converts it to a std::string */
    template <class T>
    std::string to_string(T n)
    {
        std::ostringstream output;

        output << n;
        return (output.str());
    }

    /* Takes a std::string and converts it to a type T. Needs template
       instantiation to determine type, call like: to_type<type>(str)*/
    template <class T>
    T to_type(std::string str)
    {
        T                  n;
        std::istringstream input(str);

        if (!(input >> n))
            return (T(0));
        return (n);
    }

    /* Returns a string.
        Trims from beginning to 1st occurence of charset and from last occurence
       to end */
    std::string strtrim(std::string str, const std::string charset);

    /* If the host is 'localhost', return '127.0.0.1' to be usable by some
      functions. Else return host untouched */
    std::string transform_localhost(std::string host);

    /* Get OS name "Mac OSX" or "Linux" */
    std::string getOsName();

    /* Return a string with the extension of the file (with leading '.'),
        or an empty string if there is no extension */
    std::string file_extension(std::string filename);

    bool is_dir(std::string uri_path);

    bool is_regular_file(std::string uri_path);

    /* Return if the method of the request is authorized */
    bool access_method(Config& server_config, Request& request);

    /* Just some wrapppers for file opening */
    FileHandler open_file_stream(std::string filename, Config& config,
                                 std::string mode = "r");
    FileHandler open_file_stream(int file_descriptor, Config& config,
                                 std::string mode = "r");

    std::vector<std::string> list_directory(const char* dirpath);
    std::string              to_hex(size_t nb);
    int                      to_dec(std::string hex);
    void                     read_chunk(Request& request, std::string& chunk);

} // namespace ft

#endif
