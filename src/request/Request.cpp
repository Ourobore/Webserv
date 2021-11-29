#include "Request.hpp"
#include "utilities.hpp"

Request::Request(std::string& bytes, Config& server_config)
    : _index_names(), _chunk(NULL), all_chunks_received(false)
{
    std::string              req_str = std::string(bytes);
    std::vector<std::string> req_lines;

    _location_index = -1;
    split_lines(req_str, req_lines);
    if (!req_lines.empty())
    {
        if (!parse_first_header(req_lines, server_config))
        {
            parse_body(req_lines); // Need error managment, not needed anymore
            return;
        }
        else
        {
            req_lines.erase(req_lines.begin());
            parse_headers(req_lines);

            bytes = bytes.substr(bytes.find("\r\n\r\n") + 4);
            if (tokens["Transfer-Encoding"].find("chunked") !=
                std::string::npos)
            {
                while (Chunk::creation_possible(bytes))
                {
                    _chunk = new Chunk(bytes);
                    if (_chunk->completed())
                    {
                        tokens["Body"].append(_chunk->chunk());
                        ft::add_content_length(tokens["Content-Length"],
                                               _chunk->chunk_length());
                        delete _chunk;
                    }
                }
                if (Chunk::empty_chunk(bytes))
                    all_chunks_received = true;
            }
            else
                tokens["Body"] = bytes;
        }
    }
}

void Request::parse_headers(std::vector<std::string>& req_lines)
{
    std::vector<std::string>::iterator it;
    std::vector<std::string>           words;
    std::string                        key;
    std::string                        val;

    for (it = req_lines.begin(); it != req_lines.end(); ++it)
    {
        words = split_tokens(*it, " \r\n");
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

void Request::parse_body(std::vector<std::string>& req_lines)
{
    std::string                        content;
    std::vector<std::string>::iterator it;
    for (it = req_lines.begin(); it != req_lines.end(); ++it)
    {
        content.append((*it).c_str(), (*it).length());
        // Body needs the \n, or the content will be on one line
        if (it + 1 != req_lines.end())
            content.append("\n");
    }

    tokens["Body"] = content;
}

// Split the request line by line at '\n'
void Request::split_lines(std::string&              req_str,
                          std::vector<std::string>& req_lines)
{
    std::istringstream iss(req_str);
    std::string        line;

    while (std::getline(iss, line))
        req_lines.push_back(line);
}

// Split line and extract tokens in a vector
std::vector<std::string> Request::split_tokens(std::string line,
                                               std::string delimiters)
{
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

int Request::parse_first_header(std::vector<std::string>& req_lines,
                                Config&                   server_config)
{
    static const std::string types[9] = {"GET",     "POST",  "DELETE",
                                         "HEAD",    "PUT",   "CONNECT",
                                         "OPTIONS", "TRACE", "PATCH"};

    std::vector<std::string> words = split_tokens(req_lines[0], " \r\n");
    if (words.size() > 1 && words.size() <= 9)
    {
        // Check method type
        for (int i = 0; i < 9; ++i)
        {
            if (words[0] == types[i]) // Method is valid
            {
                tokens["Method"] = words[0];
                break;
            }
        }
        if (tokens["Method"].empty())
            return 0;

        tokens["Request-URI"] = words[1];

        // parse query string
        size_t pos = tokens["Request-URI"].find_last_of('?');
        if (pos != std::string::npos)
        {
            tokens["Query-string"] = tokens["Request-URI"].substr(pos + 1);
            tokens["Request-URI"].erase(pos);
        }
        parse_uri(server_config);

        // Resolve Path-translated
        if (_location_index != -1)
        {
            tokens["Path-Translated"] =

                server_config.get_locations()[_location_index].get_root() +
                tokens["Pathinfo"];
        }
        else
        {
            tokens["Path-Translated"] =
                server_config.get_root() + tokens["Pathinfo"];
        }
        if (tokens["Pathinfo"].empty())
            tokens["Pathinfo"] = "/";

        if (words.size() == 3)
            tokens["Protocol"] = words[2];

        return 1;
    }

    return 0;
}

void Request::resolve_index()
{
    std::string uri_path = ft::strtrim(tokens["URI"], "/");

    for (size_t i = 0; i < _index_names.size(); i++)
    {
        if (ft::is_regular_file(tokens["URI"] + "/" + _index_names[i]))
        {
            uri_path = uri_path + "/" + _index_names[i];
            tokens["URI"] = uri_path;
            break;
        }
    }
}

void Request::parse_uri(Config& server_config)
{
    std::string           tmp = tokens["Request-URI"];
    size_t                back_pos = 0;
    std::vector<Location> locations = server_config.get_locations();
    _location_index = -1;

    while (back_pos != std::string::npos)
    {
        for (size_t i = 0; i < locations.size(); i++)
        {
            if (tmp == locations[i].get_path())
            {
                _location_index = i;

                // Concatenate root + client request uri
                if (!locations[i].get_root().empty())
                {
                    tokens["URI"] = ft::strtrim(locations[i].get_root(), "/") +
                                    "/" + ft::strtrim(tmp, "/");
                    std::string substr = tokens["Request-URI"].substr(
                        tokens["Request-URI"].find(tmp) + tmp.length());
                    tokens["URI"].append(substr);

                    // Split Pathinfo from URI
                    while (!ft::is_regular_file(tokens["URI"]))
                    {
                        size_t pos = tokens["URI"].find_last_of('/');
                        if (pos != std::string::npos)
                        {
                            tokens["Pathinfo"].insert(
                                0, tokens["URI"].substr(pos));
                            tokens["URI"].erase(pos);
                        }
                        else
                        {
                            tokens["URI"].append(tokens["Pathinfo"]);
                            break;
                        }
                    }
                }
                if (ft::is_dir(tokens["URI"]))
                {
                    // Append location{index} list
                    std::vector<std::string> indexes = locations[i].get_index();
                    for (size_t j = 0; j < indexes.size(); j++)
                        _index_names.push_back(indexes[j]);

                    // Append server{index} list
                    indexes = server_config.get_index();
                    for (size_t j = 0; j < indexes.size(); j++)
                        _index_names.push_back(indexes[j]);
                    resolve_index();
                }
                break;
            }
        }
        // Si on a pas trouve de location correspondant, on retente en enlevant
        // une precision
        if (tokens["URI"].empty())
        {
            back_pos = tmp.find_last_of("/");
            if (back_pos != std::string::npos)
                tmp.erase(back_pos);
        }
        else
            back_pos = std::string::npos;
    }
    if (tokens["URI"].empty())
    {
        tokens["URI"] = ft::strtrim(server_config.get_root(), "/") + "/" +
                        ft::strtrim(tokens["Request-URI"], "/");
        _index_names = server_config.get_index();
        resolve_index();
    }
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
    return (_index_names);
}

int Request::location_index() const
{
    return (_location_index);
}

Chunk* Request::chunk()
{
    return (_chunk);
}

void Request::set_chunk(Chunk* chunk)
{
    _chunk = chunk;
}