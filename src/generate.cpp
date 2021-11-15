#include "generate.hpp"

// clang-format off

std::string generate::status_message(int status_code)
{
    std::string message("");

    switch (status_code)
    {
        case 200: message = "OK";
        case 202: message = "Accepted";
        case 204: message = "No Content";
        case 301: message = "Moved Permanently";
        case 400: message = "Bad Request";
        case 403: message = "Forbidden";
        case 404: message = "Not Found";
        case 405: message = "Method Not Allowed";
    }
    return (message);
}

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

// clang-format on