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
std::vector<FileHandler>::iterator Webserv::is_file_fd(int file_descriptor)
{
    std::vector<FileHandler>::iterator it;

    for (it = files.begin(); it != files.end(); ++it)
        if (it->fd() == file_descriptor)
            return (it);
    return (files.end());
}

/* If the destination file descriptor corresponds to a file destination fd,
   return an iterator to the FileHandler equivalent to this file. If not,
   return an end() iterator */
std::vector<FileHandler>::iterator Webserv::get_file_from_client(int dest_fd)
{
    std::vector<FileHandler>::iterator it;

    for (it = files.begin(); it != files.end(); ++it)
        if (it->dest_fd() == dest_fd)
            return (it);
    return (files.end());
}

/* Get the index that corresponds to file descriptor in the pollfd structure */
int Webserv::get_poll_index(int file_descriptor)
{
    for (size_t i = 0; i < pfds.size(); ++i)
        if (pfds[i].fd == file_descriptor)
            return (i);
    return (-1);
}
