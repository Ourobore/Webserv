#include "FileHandler.hpp"
#include "Webserv.hpp"

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler Webserv::open_file_stream(std::string filename)
{
    try
    {
        FileHandler file(filename);
        file.set_status(200);
        return (file);
    }
    catch (FileHandler::NoFile exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_404;
        error_404.set_status(404);
        return (error_404);
    }
    catch (FileHandler::OpenError exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_500;
        error_500.set_status(500);
        return (error_500);
    }
}

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler Webserv::open_file_stream(int file_descriptor)
{
    try
    {
        FileHandler file(file_descriptor);
        file.set_status(200);
        return (file);
    }
    catch (FileHandler::NoFile exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_404;
        error_404.set_status(404);
        return (error_404);
    }
    catch (FileHandler::OpenError exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_500;
        error_500.set_status(500);
        return (error_500);
    }
}