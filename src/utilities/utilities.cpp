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

std::string ft::file_extension(std::string filename)
{
    const unsigned long extension_index = filename.find_last_of(".");

    if (extension_index == std::string::npos)
        return ("");
    else
        return (filename.substr(extension_index));
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

bool ft::access_method(Config& server_config, Request& request)
{
    if (request.location_index() != -1)
    {
        Location location =
            server_config.get_locations()[request.location_index()];

        if (location.get_methods()[request.tokens["Method"]])
            return (true);
        else
            return (false);
    }

    /* If no corresponding location */
    if (request.tokens["Method"] == "GET")
        return (true);
    else
        return (false);
}

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler ft::open_file_stream(std::string filename, Config& config,
                                 std::string mode)
{
    try
    {
        FileHandler file(filename, mode);
        file.set_status(200);
        return (file);
    }
    catch (FileHandler::NoFile exception)
    {
        // std::cout << exception.what() << std::endl;
        try // If we can get the error 404 page
        {
            FileHandler error_404(config.get_error_pages()["404"], mode);
            error_404.set_status(404);
            return (error_404);
        }
        catch (...) // Else generate one
        {
            FileHandler error_404;
            error_404.set_string_output(generate::status_message(404));
            error_404.set_status(404);
            return (error_404);
        }
    }
    catch (FileHandler::IsDir exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_404(config.get_error_pages()["404"], mode);
        error_404.set_status(404);
        return (error_404);
    }
    catch (FileHandler::OpenError exception)
    {
        // std::cout << exception.what() << std::endl;
        try // If we can get the error 500 page
        {
            FileHandler error_500(config.get_error_pages()["500"], mode);
            error_500.set_status(500);
            return (error_500);
        }
        catch (...) // Else generate one
        {
            FileHandler error_500;
            error_500.set_string_output(generate::status_message(500));
            error_500.set_status(500);
            return (error_500);
        }
    }
}

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler ft::open_file_stream(int file_descriptor, Config& config,
                                 std::string mode)
{
    try
    {
        FileHandler file(file_descriptor, mode);
        file.set_status(200);
        return (file);
    }
    catch (FileHandler::NoFile exception)
    {
        // std::cout << exception.what() << std::endl;
        try // If we can get the error 404 page
        {
            FileHandler error_404(config.get_error_pages()["404"], mode);
            error_404.set_status(404);
            return (error_404);
        }
        catch (...) // Else generate one
        {
            FileHandler error_404;
            error_404.set_string_output(generate::status_message(404));
            error_404.set_status(404);
            return (error_404);
        }
    }
    catch (FileHandler::IsDir exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_404(config.get_error_pages()["404"], mode);
        error_404.set_status(404);
        return (error_404);
    }
    catch (FileHandler::OpenError exception)
    {
        // std::cout << exception.what() << std::endl;
        try // If we can get the error 500 page
        {
            FileHandler error_500(config.get_error_pages()["500"], mode);
            error_500.set_status(500);
            return (error_500);
        }
        catch (...) // Else generate one
        {
            FileHandler error_500;
            error_500.set_string_output(generate::status_message(500));
            error_500.set_status(500);
            return (error_500);
        }
    }
}

std::vector<std::string> ft::list_directory(const char* dirpath)
{
    DIR*           dir;
    struct dirent* ent;

    std::vector<std::string> ls;

    dir = opendir(dirpath);
    if (dir)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            ls.push_back(ent->d_name);
        }
        closedir(dir);
    }

    return ls;
}

std::string ft::to_hex(size_t nb)
{
    std::stringstream ss;
    ss << std::hex << nb;

    std::string res(ss.str());

    return res;
}

int ft::to_dec(std::string hex)
{
    int dec;

    std::stringstream ss;
    ss << hex;
    ss >> std::hex >> dec;

    return dec;
}

void ft::read_chunk(Request& request, std::string& chunk)
{
    while (!chunk.empty())
    {
        int crlf_pos = chunk.find("\r\n");
        int chunk_length = ft::to_dec(chunk.substr(0, crlf_pos));

        request.tokens["Body"].append(chunk.substr(crlf_pos + 2, chunk_length),
                                      chunk_length);
        chunk = chunk.substr(crlf_pos + 2 + chunk_length + 2);
        // substr out of range here sometimes
    }
}