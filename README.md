# Webserv

A non-blocking **NGINX like web server**, built in **C++** but with less functionalities. Implemented ability to serve content, upload files, custom error pages or execute CGI scripts.
The server is configured with a `nginx.conf` like file.

<br/>

## How to start the server

First, create or configure the `.conf` file in `requirements/conf`. The server needs to be restarted every time the configuration file is modified.

Then, add the content you want to serve in `requirements/html` : it can be `.html`, `.css`, `.js` or any other file a web explorer will recognize.

When all that is done, you can build and run the app with the following commands:

```
$ make
$ ./server [path_to_config_file]
```

If no `path_to_config_file` is given, the server will run with `default.conf` if found.

<br/>

## Notes

`.devcontainer`, `.vscode`, `.clang-format` and `compile_flags.txt` where files/directories needed to facilitate development. They where used to: have a common dev environment, launch debugger, and use **lldb** and **clang format**

<br/>

## Project members

- lchapren
- josaykos
- jsaguez
