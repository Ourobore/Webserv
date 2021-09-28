#include "Request.hpp"

Request::Request(char* bytes)
{

    req_str = std::string(bytes);
    split_lines();
    if (!req_lines.empty())
    {
        if (!parse_first_header())
        {
            parse_body();
            return;
        }
        else
        {
            req_lines.erase(req_lines.begin());
            parse_headers();
            parse_body();
        }
    }
}

void Request::parse_headers()
{
    std::vector<std::string>::iterator it;
    std::vector<std::string>           words;

    for (it = req_lines.begin(); it != req_lines.end(); ++it)
    {
        words = split_tokens(*it);
        if (*it == "\r")
        {
            req_lines.erase(req_lines.begin(), it + 1);
            return;
        }
    }
    req_lines.erase(req_lines.begin(), it);
}

void Request::parse_body()
{
    std::string                        content;
    std::vector<std::string>::iterator it;
    for (it = req_lines.begin(); it != req_lines.end(); ++it)
        content.append(*it);
    tokens.insert(std::pair<std::string, std::string>("Body", content));
}

// Split the request line by line at '\n'
void Request::split_lines()
{
    std::istringstream iss(req_str);
    std::string        line;

    while (std::getline(iss, line))
        req_lines.push_back(line);
}

// Split line and extract tokens in a vector
std::vector<std::string> Request::split_tokens(std::string line)
{
    std::string              delimiters = " \r\n";
    std::size_t              prev = 0, pos;
    std::vector<std::string> words;

    while ((pos = line.find_first_of(delimiters, prev)) != std::string::npos)
    {
        if (pos >= prev)
            words.push_back(line.substr(prev, pos - prev));
        prev = pos + 1;
    }

    return words;
}

int Request::parse_first_header()
{
    static const std::string types[3] = {"GET", "POST", "DELETE"};

    std::vector<std::string> words = split_tokens(req_lines[0]);
    if (words.size() == 3)
    {
        // Check method type
        for (int i = 0; i < 3; ++i)
        {
            if (words[0] == types[i]) // Method is valid
            {
                tokens.insert(
                    std::pair<std::string, std::string>("Method", words[0]));
                break;
            }
        }
        if (tokens.find("Method") == tokens.end())
            return 0;

        // Set URI and Protocol
        tokens.insert(std::pair<std::string, std::string>("URI", words[1]));
        tokens.insert(
            std::pair<std::string, std::string>("Protocol", words[2]));

        return 1;
    }

    return 0;
}
