#include "FileHandler.hpp"
#include "Webserv.hpp"

/* Just some wrappper for file opening. Useful? Avoid try catch */
FileHandler Webserv::open_file_stream(std::string filename, std::string mode)
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
FileHandler Webserv::open_file_stream(int file_descriptor, std::string mode)
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

/* If the destination file descriptor corresponds to a Client file descriptor,
   returns the first FileHandler of the Client*/
std::vector<FileHandler>::iterator Webserv::get_file_from_client(int dest_fd)
{
    ClientHandler& client = get_client(dest_fd);

    return (client.files().begin());
}

/* Get the index that corresponds to file descriptor in the pollfd structure */
int Webserv::get_poll_index(int file_descriptor)
{
    for (size_t i = 0; i < pfds.size(); ++i)
        if (pfds[i].fd == file_descriptor)
            return (i);
    return (-1);
}
