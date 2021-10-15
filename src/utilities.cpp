#include "utilities.hpp"

std::string ft::strtrim(std::string str, const std::string charset)
{
    str.erase(0, str.find_first_not_of(charset));
    str.erase(str.find_last_not_of(charset) + 1);
    return str;
}

std::string ft::transform_localhost(std::string host)
{
    if (host == "localhost")
        return ("127.0.0.1");
    else
        return (host);
}