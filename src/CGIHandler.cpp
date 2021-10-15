#include "CGIHandler.hpp"
#include "utilities.hpp"
#include <netinet/in.h>

CGIHandler::CGIHandler(Config const& config, Request const& request,
                       int client_fd)
{
    struct sockaddr_in client_address = Socket::get_socket_address(client_fd);

    // Variables that block the input file as a parameter
    // variables["GATEWAY_INTERFACE"] = "CGI/1.1";
    // variables["REQUEST_METHOD"] = request["Method"];
    // variables["SERVER_SOFTWARE"] = "Webserv";
    // variables["SERVER_NAME"] = ft::transform_localhost(config.get_host());

    // Setting up CGI variables as envp
    variables["PHP_SELF"] = request["URI"]; // Dangerous?
    variables["REDIRECT_STATUS"] = "200";
    // ^ Does the status change? Take from fastcgi_param ^

    // Server variables
    variables["DOCUMENT_ROOT"] = config.get_root();
    variables["SERVER_PROTOCOL"] = request["Protocol"];
    variables["SERVER_ADDR"] = ft::transform_localhost(config.get_host());
    variables["SERVER_PORT"] = ft::to_string(config.get_port());

    // Client variables. May need to take informations from client socket?
    variables["REMOTE_HOST"] = "localhost";
    variables["REMOTE_ADDR"] = Socket::get_socket_ip_address(client_address);
    variables["REMOTE_PORT"] =
        ft::to_string(Socket::get_socket_port(client_address));

    // Request variables
    variables["REQUEST_URI"] = request["URI"];
    variables["SCRIPT_NAME"] = request["URI"];
    variables["PATH_INFO"] = "/"; // Testing, should not use relative path
    // variables["PATH_TRANSLATED"] = "";
    // SCRIPT_FILENAME == pwd + variables["DOCUMENT_ROOT"] +
    // variables["SCRIPT_NAME"];
    variables["QUERY_STRING"] = "";
    variables["AUTH_PATH"] = request["Authorization"];
    variables["CONTENT_TYPE"] = request["Content-Type"];
    variables["CONTENT_LENGTH"] = request["Content-Length"];

    // HTTP variables
    variables["HTTP_HOST"] = request["Host"];
    variables["HTTP_ACCEPT"] = request["Accept"];
    variables["HTTP_CONNECTION"] = request["Connection"];
    variables["HTTP_USER_AGENT"] = request["User-Agent"];
    variables["HTTP_ACCECPT_ENCODING"] = request["Accept-Encoding"];
    variables["HTTP_ACCECPT_LANGUAGE"] = request["Accept-Language"];
    variables["HTTPS"] = ""; // Really needed?

    env_array = CGIHandler::get_env_array();

    // Setting CGI binary and script paths
    char*       buf = getcwd(NULL, 0);
    std::string pwd(buf);
    free(buf);

    cgi_path = pwd + "/cgi-bin/php-cgi"; // Need fastcgi_pass in config
    script_name = variables["SCRIPT_NAME"].erase(0, 1);
    root_directory = pwd + variables["DOCUMENT_ROOT"];

    // Debug: Printing env_array
    DEBUG_print_env_array();

    // Setting CGI arguments for execve()
    cgi_argv = new char*[3]();

    cgi_argv[0] = new char[cgi_path.length() + 1]();
    memcpy(cgi_argv[0], cgi_path.c_str(), cgi_path.length());

    cgi_argv[1] = new char[script_name.length() + 1]();
    memcpy(cgi_argv[1], script_name.c_str(), script_name.length());
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
        chdir(root_directory.c_str());
        execve(cgi_path.c_str(), cgi_argv, env_array);
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

// Debug. Printing env_array containing CGI environment variables
void CGIHandler::DEBUG_print_env_array() const
{
    std::cout << "========== Start ENV_ARRAY ==========" << std::endl;

    std::map<std::string, std::string>::const_iterator it;
    int                                                i = 0;
    for (it = variables.begin(); it != variables.end(); ++it, ++i)
        std::cout << env_array[i] << std::endl;

    std::cout << "========== End ENV_ARRAY ==========" << std::endl;
}