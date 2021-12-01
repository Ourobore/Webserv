#include "generate.hpp"
#include "utilities.hpp"

// clang-format off

std::string generate::error_page(int status_code)
{
    std::ostringstream output;

    output << "<html>\r\n"
           << "  <head>\r\n"
           << "    <title>" << status_code << " " << status_message(status_code) << "</title>\r\n"
           << "  </head > \r\n"
           << "  <body>\r\n"
           << "    <center><h1>" << status_code << " " << status_message(status_code) << "</h1>" << "</center>\r\n"
           << "    <hr/>\r\n"
           << "    <center>webserv/42</center>\r\n"
           << "    <center>(Generated)</center>\r\n"
           << "  </body>\r\n"
           << "</html>\r\n\r\n";

    return (output.str());
}

std::string generate::file_deleted()
{
    std::ostringstream output;

    output << "<html>\r\n"
           << "  <body>\r\n"
           << "    <h1>File deleted.</h1>\r\n"
           << "  </body>\r\n"
           << "</html>\r\n\r\n";

    return (output.str());
}

std::string generate::autoindex(Request& request)
{
    std::vector<std::string> ls = ft::list_directory(request["URI"].c_str());
    std::vector<std::string>::iterator it;

    std::ostringstream output;

    output << "<html>\r\n"
           << "  <head>\r\n"
           << "    <title>Index of " << request["Request-URI"] << "</title>\r\n"
           << "  </head > \r\n"
           << "  <body>\r\n"
           << "    <h1>Index of " << request["Request-URI"] << "</h1>\r\n"
           << "    <hr/>\r\n";
    
    std::string back = request["Request-URI"].substr(0, request["Request-URI"].find_last_of('/'));
    if (back.empty()) {
        back = "/";
    }
    output << "    <pre><a href=" << "\"" << back << "\"" << ">../</a>" << std::endl;

    for (it = ls.begin(); it != ls.end(); ++it) {
        if (*it != "/." && *it != "/.."){
            std::string href;

            if (request["Request-URI"] != "/")
                href = request["Request-URI"] + *it;
            else
                href = *it;
            output<< "<a href=\"" << href << "\">" << (*it).substr(1) << "</a>" << std::endl;
        }
    }
    output << "    </pre>\r\n"
           << "    <hr/>\r\n"
           << "  </body>\r\n"
           << "</html>\r\n\r\n";

    return (output.str());
}

void generate::response(ClientHandler& client, int status_code)
{
    client.response().code = status_code;
    client.response().content_type = "text/html";
    client.response().content = generate::error_page(status_code);
    client.set_date();
}

std::string generate::status_message(int status_code)
{
    std::string message("");

    switch (status_code)
    {
        case 200: 
            message = "OK";
            break;
        case 202: 
            message = "Accepted";
            break;
        case 204: 
            message = "No Content";
            break;
        case 301: 
            message = "Moved Permanently";
            break;
        case 400: 
            message = "Bad Request";
            break;
        case 403: 
            message = "Forbidden";
            break;
        case 404: 
            message = "Not Found";
            break;
        case 405: 
            message = "Method Not Allowed";
            break;
        case 413: 
            message = "Payload Too Large";
            break;
        case 500:
            message = "Internal Server Error";
            break;
    }
    return (message);
}

// clang-format on