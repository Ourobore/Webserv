#include "CGIHandler.hpp"
#include "utilities.hpp"
#include <netinet/in.h>

CGIHandler::CGIHandler(Config& config, Request& request, int client_fd)
    : input_pipe(NULL), output_pipe(NULL)
{
    struct sockaddr_in client_address = Socket::get_socket_address(client_fd);

    // Variables that block the input file as a parameter
    variables["GATEWAY_INTERFACE"] = "CGI/1.1";
    variables["REQUEST_METHOD"] = request["Method"];
    variables["SERVER_SOFTWARE"] = "Webserv";
    variables["SERVER_NAME"] = ft::transform_localhost(config.get_host());

    // Setting up CGI variables as envp
    variables["REDIRECT_STATUS"] = "200";

    // Server variables
    variables["SERVER_PROTOCOL"] = request["Protocol"];
    variables["SERVER_PORT"] = ft::to_string(config.get_port()[0]);

    // Client variables. May need to take informations from client socket?
    variables["REMOTE_HOST"] = "localhost";
    variables["REMOTE_ADDR"] = Socket::get_socket_ip_address(client_address);
    variables["REMOTE_IDENT"];
    variables["REMOTE_USER"];

    // Request variables
    variables["SCRIPT_NAME"] = request["URI"];
    variables["PATH_INFO"] = request["Pathinfo"];
    variables["PATH_TRANSLATED"] = request["Path-Translated"];
    variables["QUERY_STRING"] = request["Query-string"];
    variables["AUTH_TYPE"] = request["Authorization"];
    variables["CONTENT_TYPE"] = request["Content-Type"];
    variables["CONTENT_LENGTH"] = request["Content-Length"];

    // HTTP variables
    variables["HTTP_HOST"] = request["Host"];
    variables["HTTP_ACCEPT"] = request["Accept"];
    variables["HTTP_CONNECTION"] = request["Connection"];
    variables["HTTP_USER_AGENT"] = request["User-Agent"];
    variables["HTTP_ACCECPT_ENCODING"] = request["Accept-Encoding"];
    variables["HTTP_ACCECPT_LANGUAGE"] = request["Accept-Language"];

    if (config.get_locations()[request.location_index()].get_cgi_extension() ==
        ".php")
        variables["SCRIPT_FILENAME"] = request["URI"];

    env_array = CGIHandler::get_env_array();

    // Setting CGI binary and script paths

    cgi_path = config.get_locations()[request.location_index()].get_cgi_pass();
    script_name = variables["SCRIPT_NAME"];
    char pwd[1024] = {0};
    root_directory = getcwd(pwd, 1024) +
                     request["URI"].substr(0, request["URI"].find_last_of("/"));

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

    if (input_pipe)
    {
        if (input_pipe[PIPEREAD])
            close(input_pipe[PIPEREAD]);
        if (input_pipe[PIPEWRITE])
            close(input_pipe[PIPEWRITE]);
        delete[] input_pipe;
    }

    if (output_pipe[PIPEREAD])
        close(output_pipe[PIPEREAD]);
    if (output_pipe[PIPEWRITE])
        close(output_pipe[PIPEWRITE]);
    delete[] output_pipe;
}

void CGIHandler::setup_cgi(ClientHandler&              client,
                           std::vector<struct pollfd>& pfds,
                           Config&                     server_config)
{
    if (client.request()->tokens["Method"] == "POST")
        input_pipe = new int[2]();
    output_pipe = new int[2]();

    if (input_pipe && pipe(input_pipe) == -1)
    {
        if (input_pipe)
            delete[] input_pipe;
        perror("Error: couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }

    if (pipe(output_pipe) == -1)
    {
        if (output_pipe)
            delete[] output_pipe;
        perror("Error: couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }

    // Must add input pipe fd to files and pfds, and CGIHandler to client
    if (input_pipe)
    {
        FileHandler cgi_pipe_input =
            ft::open_file_stream(input_pipe[PIPEWRITE], server_config, "w");

        client.files().push_back(cgi_pipe_input);
        struct pollfd pfd_input = {cgi_pipe_input.fd(), POLLOUT, 0};
        pfds.push_back(pfd_input);
    }

    // Must add output pipe fd to files and pfds, and CGIHandler to client
    FileHandler cgi_pipe_output =
        ft::open_file_stream(output_pipe[PIPEREAD], server_config);

    client.files().push_back(cgi_pipe_output);
    struct pollfd pfd_output = {cgi_pipe_output.fd(), POLLIN, 0};
    pfds.push_back(pfd_output);

    client.set_cgi(this);
}

void CGIHandler::launch_cgi()
{
    int childpid = 0;

    childpid = fork();
    if (childpid == -1)
    {
        perror("Error: couldn't fork\n");
        exit(EXIT_FAILURE);
    }

    if (childpid == 0)
    {
        if (input_pipe)
        {
            dup2(input_pipe[PIPEREAD], STDIN);
            close(input_pipe[PIPEWRITE]);
        }
        dup2(output_pipe[PIPEWRITE], STDOUT);
        close(output_pipe[PIPEREAD]);
        chdir(root_directory.c_str());
        execve(cgi_path.c_str(), cgi_argv, env_array);
        perror("Error: CGI execution failed\n");
        exit(EXIT_FAILURE);
    }
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
