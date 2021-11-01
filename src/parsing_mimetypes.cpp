#include "parsing_mimetypes.hpp"
#include <cctype>

int start_from_opening_bracket(std::string& str)
{
    size_t i = str.find("types");

    if (i == std::string::npos)
        return -1;
    if (i == 0 || isspace(str[i - 1]))
    {
        i += 5;
        while (str[i])
        {
            if (str[i] != '{' && !isspace(str[i]))
                return -1;
            else if (str[i] == '{')
                break;
            i++;
        }
        if (!str[i])
            return -1;
    }
    str = str.substr(i + 1, str.length());
    return 0;
}

std::vector<std::string> split_tokens(std::string line)
{
    std::string              delimiters = " \r\n";
    std::size_t              prev = 0, pos;
    std::vector<std::string> words;

    while ((pos = line.find_first_of(delimiters, prev)) != std::string::npos)
    {
        if (pos >= prev)
        {
            std::string token = line.substr(prev, pos - prev);
            if (!token.empty())
                words.push_back(token);
        }
        prev = pos + 1;
    }
    if (prev < line.length())
        words.push_back(line.substr(prev, std::string::npos));

    return words;
}

void split_lines(std::string& str, std::vector<std::string>& lines)
{
    std::istringstream iss(str);
    std::string        line;

    while (std::getline(iss, line, ';'))
        lines.push_back(line);
}

void get_values(std::string& str, std::map<std::string, std::string>& mimetypes)
{
    std::vector<std::string>           lines;
    std::vector<std::string>           words;
    std::vector<std::string>::iterator it;

    split_lines(str, lines);

    for (size_t i = 0; i < lines.size(); i++)
    {
        words = split_tokens(lines[i]);
        for (it = words.end() - 1; it != words.begin(); --it)
            mimetypes[*it] = *words.begin();
    }
    return;
}

int ConfParsing::parsing_mimetypes(
    const char* filename, std::map<std::string, std::string>& mimetypes)
{
    // Open file
    std::stringstream sbuf;
    std::fstream      fs(filename);

    if (fs.is_open())
        sbuf << fs.rdbuf();
    else
        return -1;
    std::string str = sbuf.str();

    if (start_from_opening_bracket(str) == -1)
        return -1;

    // find closing bracket
    if (str.find("}") == std::string::npos)
        return -1;

    std::string::iterator it;
    get_values(str, mimetypes);

    return 0;
}