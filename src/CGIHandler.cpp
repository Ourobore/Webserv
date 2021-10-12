#include "CGIHandler.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "utilities.hpp"

CGIHandler::CGIHandler(Config const& config, Request const& request)
{
    // Variables that block the input file as a parameter
    // variables["GATEWAY_INTERFACE"] = "CGI/1.1";
    // variables["REQUEST_METHOD"] = request["Method"];
    // variables["SERVER_SOFTWARE"] = "Webserv";
    // variables["SERVER_NAME"] = config.get_host();

    // Setting up CGI variables as envp
    variables["PHP_SELF"] = request["URI"]; // Dangerous?
    variables["REDIRECT_STATUS"] = "200";
    // ^ Does the status change? Take from fastcgi_param ^

    // Request variables
    variables["REQUEST_URI"] = request["URI"];
    variables["SCRIPT_NAME"] = request["URI"];
    variables["PATH_INFO"] = "/"; // Testing, should not use relative path
    // variables["PATH_TRANSLATED"] = "";
    variables["QUERY_STRING"] = "";
    variables["AUTH_PATH"] = request["Authorization"];
    variables["CONTENT_TYPE"] = request["Content-Type"];
    variables["CONTENT_LENGTH"] = request["Content-Length"];

    // Server variables
    variables["DOCUMENT_ROOT"] = config.get_root();
    variables["SERVER_PROTOCOL"] = request["Protocol"];
    variables["SERVER_ADDR"] = "127.0.0.1";
    variables["SERVER_PORT"] = ft::to_string(config.get_port());

    // Client variables. May need to take informations from client socket?
    variables["REMOTE_HOST"] = "localhost";
    variables["REMOTE_ADDR"] = "127.0.0.1";
    variables["REMOTE_PORT"] = ft::to_string(config.get_port());

    // HTTP variables
    variables["HTTP_HOST"] = request["Host"];
    variables["HTTP_ACCEPT"] = request["Accept"];
    variables["HTTP_CONNECTION"] = request["Connection"];
    variables["HTTP_USER_AGENT"] = request["User-Agent"];
    variables["HTTP_ACCECPT_ENCODING"] = request["Accept-Encoding"];
    variables["HTTP_ACCECPT_LANGUAGE"] = request["Accept-Language"];
    variables["HTTPS"] = ""; // Really needed?

    env_array = CGIHandler::get_env_array();

    // Debug. Printing env_array
    std::cout << "========================" << std::endl;
    std::map<std::string, std::string>::iterator it;
    int                                          i = 0;
    for (it = variables.begin(); it != variables.end(); ++it, ++i)
        std::cout << env_array[i] << std::endl;
    std::cout << "========================" << std::endl;

    // Setting CGI arguments for execve()
    cgi_argv = new char*[3]();
    std::string path_info("cgi-bin/php-cgi-osx"); // Need fastcgi_pass in config
    std::string script_filename("html" + request["URI"]);

    cgi_argv[0] = new char[path_info.length() + 1]();
    memcpy(cgi_argv[0], path_info.c_str(), path_info.length());

    cgi_argv[1] = new char[script_filename.length() + 1]();
    memcpy(cgi_argv[1], script_filename.c_str(), script_filename.length());

    // Need cgi_argv[2] = NULL ?
}

CGIHandler::~CGIHandler()
{
    for (size_t i = 0; i < variables.size(); ++i)
        delete[] env_array[i];
    delete[] env_array;

    delete[] cgi_argv[0];
    delete[] cgi_argv[1];
    delete[] cgi_argv;
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
        perror("Error: couldn't fork\n");
        exit(EXIT_FAILURE);
    }

    if (childpid == 0)
    {
        dup2(pipefd[PIPEWRITE], STDOUT);
        close(pipefd[PIPEREAD]);
        execve(cgi_argv[0], cgi_argv, env_array);
        perror("Error: CGI execution failed\n");
        exit(EXIT_FAILURE);
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

    char** env_array = new char*[variables.size() + 1]();

    int i = 0;
    for (it = variables.begin(); it != variables.end(); ++it, ++i)
    {
        std::string pair = it->first + "=" + it->second;

        env_array[i] = new char[pair.length() + 1]();
        memcpy(env_array[i], pair.c_str(), pair.length());
    }
    return (env_array);
}
