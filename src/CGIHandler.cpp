#include "CGIHandler.hpp"
#include "Config.hpp"
#include "Request.hpp"

CGIHandler::CGIHandler(Request request)
{
    variables["GATEWAY_INTERFACE"] = "CGI/1.1";
    variables["SERVER_PROTOCOL"] = "HTTP/1.1";
    variables["SERVER_SOFTWARE"] = "Webserv";
    variables["PATH_INFO"] =
        "./cgi-bin/php-cgi"; // Testing, should not use relative path
    variables["SCRIPT_FILENAME"] = "./html" + request["URI"];
    variables["REDIRECT_STATUS"] = "200"; // Should the status of the request,
                                          // we don't know if it is a redirect
}

CGIHandler::~CGIHandler()
{
}

void CGIHandler::execute(char buffer[30000]) // Need changes i think
{
    int pipefd[2];
    int childpid = 0;

    if (pipe(pipefd) == -1)
    {
        perror("Error: couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }

    childpid = fork();
    if (childpid == -1)
    {
        perror("Error: couldn't fork");
        exit(EXIT_FAILURE);
    }

    if (childpid == 0)
    {
        std::string path_info(variables["PATH_INFO"]);

        char* cgi_executable[2];
        memset(cgi_executable, 0, 2);
        memcpy(cgi_executable[0], path_info.c_str(), path_info.length() + 1);

        dup2(pipefd[PIPEWRITE], STDOUT);
        close(pipefd[PIPEREAD]);
        execve(cgi_executable[0], cgi_executable, CGIHandler::get_env_array());
        perror("Error: CGI execution failed");
    }
    else
    {
        waitpid(childpid, NULL, 0);
        close(pipefd[PIPEWRITE]);
        read(pipefd[PIPEREAD], buffer, 30000); // Need changes i think
        close(pipefd[PIPEREAD]);
        return;
    }
}

char** CGIHandler::get_env_array()
{
    std::map<std::string, std::string>::iterator it;

    char** env_array = new char*[variables.size() + 1];
    memset(env_array, 0, variables.size() + 1);

    int i = 0;
    for (it = variables.begin(); it != variables.end(); ++it, ++i)
    {
        std::string pair = it->first + "=" + it->second;

        env_array[i] = new char[pair.length() + 1];
        memset(env_array[i], 0, pair.length() + 1);
        memcpy(env_array[i], pair.c_str(), pair.length() + 1);
    }
    return (env_array);
}