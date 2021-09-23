#include "Request.hpp"

Request::Request(char* bytes)
{
    reqstr = std::string(bytes);

    std::vector<std::string> tokens;

    tokens = split_tokens();

    std::vector<std::string>::iterator it;
    // std::cout << "TOKENS" << std::endl;
    // for (it = tokens.begin(); it != tokens.end(); ++it)
    // {
    // std::cout << *it << std::endl;
    // }
    method = tokens[0];
    url = tokens[1];
}

std::vector<std::string> Request::split_tokens()
{
    std::string              delimiters = " \r\n";
    std::size_t              prev = 0, pos;
    std::vector<std::string> tokens;

    while ((pos = reqstr.find_first_of(delimiters, prev)) != std::string::npos)
    {
        if (pos > prev)
        {

            tokens.push_back(reqstr.substr(prev, pos - prev));
            int i;
            while ((i = delimiters.find(reqstr[pos + 1])) >= 0)
            {
                if (i > 0)
                {
                    pos++;
                }
            }
            prev = pos + 1;
        }
    }
    if (prev < reqstr.length())
    {
        tokens.push_back(reqstr.substr(prev, std::string::npos));
    }
    return tokens;
}