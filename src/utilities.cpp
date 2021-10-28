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

std::string ft::getOsName()
{
#ifdef __linux__
    return "Linux";
#elif __APPLE__ || __MACH__
    return "Mac OSX";
#else
    return "Other";
#endif
}

bool ft::is_dir(std::string uri_path)
{
    int fd;

    if ((fd = open(uri_path.c_str(), O_DIRECTORY)) > 0)
    {
        close(fd);
        return true;
    }
    return false;
}

bool ft::is_regular_file(std::string uri_path)
{
    int fd;

    if (!ft::is_dir(uri_path))
    {
        if ((fd = open(uri_path.c_str(), O_RDONLY)) > 0)
        {
            close(fd);
            return true;
        }
    }
    return false;
}