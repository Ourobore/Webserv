#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <sstream>
#include <string>

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
        Trims after 1st occurence to last occurence of char in charset */
    std::string strtrim(std::string str, const std::string charset);

    /* If the host is 'localhost', return '127.0.0.1' to be usable by some
      functions. Else return host untouched */
    std::string transform_localhost(std::string host);
} // namespace ft

#endif
