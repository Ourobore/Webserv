#include "generate.hpp"

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
           << "    <center>webserv/42</center >\r\n"
           << "  </body>\r\n"
           << "</html>\r\n";

    return (output.str());
}

std::string generate::file_deleted()
{
    std::ostringstream output;

    output << "<html>\r\n"
           << "  <body>\r\n"
           << "    <h1>File deleted.</h1>\r\n"
           << "  </body>\r\n"
           << "</html>\r\n";

    return (output.str());
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
    }
    return (message);
}

// clang-format on