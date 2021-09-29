#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
int main()
{
    int childpid;

    char* name[2];
    name[0] = strdup("./php-cgi");
    name[1] = 0;

    char* env[6];
    env[0] = strdup("SCRIPT_FILENAME=./html/index.php");
    env[1] = strdup("REQUEST_METHOD=GET");
    env[2] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    env[3] = strdup("PATH_INFO=/");
    env[4] = strdup("REDIRECT_STATUS=200");
    env[5] = 0;

    childpid = fork();
    if (childpid == 0)
        execve(name[0], name, env);
    else
    {
        waitpid(childpid, NULL, 0);
        return 0;
    }
}