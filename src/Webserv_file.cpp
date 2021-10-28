#include "ClientHandler.hpp"
#include "FileHandler.hpp"
#include "Webserv.hpp"

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler Webserv::open_file_stream(std::string filename, Config& config,
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
        FileHandler error_404(config.get_error_pages()["404"], mode);
        // FileHandler error_404;
        error_404.set_status(404);
        return (error_404);
    }
    catch (FileHandler::OpenError exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_500(config.get_error_pages()["500"], mode);
        // FileHandler error_500;
        error_500.set_status(500);
        return (error_500);
    }
}

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler Webserv::open_file_stream(int file_descriptor, Config& config,
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
        FileHandler error_404(config.get_error_pages()["404"], mode);
        error_404.set_status(404);
        return (error_404);
    }
    catch (FileHandler::OpenError exception)
    {
        // std::cout << exception.what() << std::endl;
        FileHandler error_500(config.get_error_pages()["500"], mode);
        error_500.set_status(500);
        return (error_500);
    }
}

/* If the file descriptor corresponds to a file, return an iterator to the
   FileHandler equivalent to this file. If not, return an end() iterator */
FileHandler* Webserv::is_file_fd(int file_descriptor)
{
    std::vector<ClientHandler>::iterator client_it;

    for (client_it = clients.begin(); client_it != clients.end(); ++client_it)
    {
        std::vector<FileHandler>::iterator file_it;
        for (file_it = client_it->files().begin();
             file_it != client_it->files().end(); ++file_it)
            if (file_it->fd() == file_descriptor)
                return (&(*file_it));
    }
    return (NULL);
}

ClientHandler& Webserv::get_client_from_file(int file_descriptor)
{
    std::vector<ClientHandler>::iterator client_it;

    for (client_it = clients.begin(); client_it != clients.end(); ++client_it)
    {
        std::vector<FileHandler>::iterator file_it;
        for (file_it = client_it->files().begin();
             file_it != client_it->files().end(); ++file_it)
            if (file_it->fd() == file_descriptor)
                return (*client_it);
    }
    return (*clients.end());
}
