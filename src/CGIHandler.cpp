#include "CGIHandler.hpp"
#include "utilities.hpp"
#include <netinet/in.h>

CGIHandler::CGIHandler(Config& config, Request& request, int client_fd)
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

    /*
    if (ft::getOsName() == "Mac OSX")
        cgi_path = pwd + "/cgi-bin/php-cgi-osx";
    else
        cgi_path = pwd + "/cgi-bin/php-cgi"; // Need fastcgi_pass in config
    */
    cgi_path = config.get_locations()[request.location_index()].get_cgi_pass();
    script_name = variables["SCRIPT_NAME"].erase(0, 1);
    // root_directory = pwd + variables["DOCUMENT_ROOT"];

    // Debug: Printing env_array
    // DEBUG_print_env_array();

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

void CGIHandler::launch_cgi(ClientHandler&              client,
                            std::vector<struct pollfd>& pfds,
                            Config&                     server_config)
{
    int         output_pipe[2];
    int         childpid = 0;
    std::string cgi_output(""); // Will be removed

    if (pipe(output_pipe) == -1)
    {
        perror("Error: couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }

    // Must add pipe fd to files and pfds
    FileHandler cgi_pipe_output =
        Webserv::open_file_stream(output_pipe[PIPEREAD], server_config);
    struct pollfd pfd = {cgi_pipe_output.fd(), POLLIN, 0};

    client.files().push_back(cgi_pipe_output.fd());
    pfds.push_back(pfd);

    childpid = fork();
    if (childpid == -1)
    {
        perror("Error: couldn't fork\n");
        exit(EXIT_FAILURE);
    }

    if (childpid == 0)
    {
        dup2(output_pipe[PIPEWRITE], STDOUT);
        close(output_pipe[PIPEREAD]);
        chdir(root_directory.c_str());
        execve(cgi_path.c_str(), cgi_argv, env_array);
        perror("Error: CGI execution failed\n");
        exit(EXIT_FAILURE);
    }
    // Below, waiting for pipe fd in poll to be readable
    // else
    // {
    //     waitpid(childpid, NULL, 0);
    //     close(output_pipe[PIPEWRITE]);
    //     /*
    //     FileHandler cgi_stream = Webserv::open_file_stream(pipefd[PIPEREAD]);
    //     if (cgi_stream.stream())
    //         cgi_output = cgi_stream.read_all();
    //     */
    //     // read(pipefd[PIPEREAD], buffer, 30000); // Need changes i think
    //     close(output_pipe[PIPEREAD]);
    // }
    // return (cgi_output);
}

bool CGIHandler::is_cgi_file(std::string filename, int location_index,
                             Config& server_config)
{
    if (location_index == -1)
        return (false);

    Location location = server_config.get_locations()[location_index];

    if (ft::file_extension(filename) == ".php" &&
        location.get_cgi_extension() == ".php")
        return (true);
    else
        return (false);
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
