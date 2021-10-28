#include "Request.hpp"
#include "utilities.hpp"

Request::Request(const char* bytes, Config& server_config) : _index_names()
{

    req_str = std::string(bytes);
    split_lines();
    if (!req_lines.empty())
    {
        if (!parse_first_header(server_config))
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
    std::string                        key;
    std::string                        val;

    for (it = req_lines.begin(); it != req_lines.end(); ++it)
    {
        words = split_tokens(*it);
        val = "";
        if (words.size() >= 2)
        {
            if (*(words[0].end() - 1) == ':')
                key = words[0].substr(0, words[0].length() - 1);
            else
                key = words[0];
            for (size_t i = 1; i < words.size(); i++)
                val.append(words[i]);

            tokens[key] = val;
        }
        else
        {
            if (*it == "\r")
            {
                req_lines.erase(req_lines.begin(), it + 1);
                return;
            }

            tokens[words[0]] = "";
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

    tokens["Body"] = content;
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
    if (prev < line.length())
        words.push_back(line.substr(prev, std::string::npos));

    return words;
}

int Request::parse_first_header(Config& server_config)
{
    static const std::string types[3] = {"GET", "POST", "DELETE"};

    std::vector<std::string> words = split_tokens(req_lines[0]);
    if (words.size() > 1 && words.size() <= 3)
    {
        // Check method type
        for (int i = 0; i < 3; ++i)
        {
            if (words[0] == types[i]) // Method is valid
            {
                tokens["Method"] = words[0];
                break;
            }
        }
        if (tokens["Method"].empty())
            return 0;

        // URI: "/dossier1/dossier2/filename/{pathinfo}?querystring"
        // 1. /dossier1/dossier2
        // 2. /dossier1
        // 3. /
        // 4. root + filename
        // 5. pathinfo
        // 6. querystring
        (void)server_config;
        tokens["Request-URI"] = words[1];
        parse_uri(server_config);
        if (words.size() == 3)
            tokens["Protocol"] = words[2];

        return 1;
    }

    return 0;
}

void Request::parse_uri(Config& server_config)
{
    std::vector<Location> locations = server_config.get_locations();
    /**************************************** DEBUG *************************/
    std::cout << "req_uri: " << tokens["Request-URI"] << std::endl;
    /**************************************** DEBUG END **********************/

    for (size_t i = 0; i < locations.size(); i++)
    {
        if (tokens["Request-URI"] == locations[i].get_path())
        {
            // Concatenate root + client request uri
            if (!locations[i].get_root().empty())
                tokens["URI"] = ft::strtrim(locations[i].get_root(), "/") +
                                "/" + ft::strtrim(tokens["Request-URI"], "/");
            else
                tokens["URI"] = ft::strtrim(server_config.get_root(), "/") +
                                "/" + ft::strtrim(tokens["Request-URI"], "/");

            // Append location{index} list
            std::vector<std::string> indexes = locations[i].get_index();
            for (size_t j = 0; j < indexes.size(); j++)
                _index_names.push_back(indexes[j]);

            // Append server{index} list
            indexes = server_config.get_index();
            for (size_t j = 0; j < indexes.size(); j++)
                _index_names.push_back(indexes[j]);
            break;
        }
    }
    if (tokens["URI"].empty())
    {
        tokens["URI"] = ft::strtrim(server_config.get_root(), "/") + "/" +
                        ft::strtrim(tokens["Request-URI"], "/");
        _index_names = server_config.get_index();
    }

    /**************************************** DEBUG *************************/
    std::cout << "tokens[\"URI\"]: " << tokens["URI"] << std::endl;
    for (size_t i = 0; i < _index_names.size(); i++)
        std::cout << "index " << i << ": " << _index_names[i] << std::endl;
    /**************************************** DEBUG END **********************/
}

std::string Request::operator[](const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it;

    it = tokens.find(key);
    if (it == tokens.end())
        return ("");
    else
        return (it->second);
}

std::vector<std::string>& Request::index_names()
{
    return _index_names;
}