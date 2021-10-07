#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <sstream>

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
} // namespace ft

#endif
