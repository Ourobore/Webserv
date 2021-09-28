#include "Request.hpp"

Request::Request(char* bytes)
{
    const std::string types[] = {"GET", "POST", "DELETE"};

    req_str = std::string(bytes);
    split_lines();
}

void Request::split_lines()
{
    std::istringstream iss(req_str);
    std::string        line;
    while (std::getline(iss, line))
    {
        req_lines.push_back(line);
    }
}
