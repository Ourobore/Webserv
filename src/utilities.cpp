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
