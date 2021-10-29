#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
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

} // namespace ft

#endif
