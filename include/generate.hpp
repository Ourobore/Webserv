#ifndef GENERATE_HPP
#define GENERATE_HPP

#include <iostream>
#include <sstream>

/* Functions in this namespace return a generated string equivalent to the one
   of a file of the same utility */
namespace generate
{
    /* Return the status message corresponding to the status code */
    std::string status_message(int status_code);

    /* Generate a default error page corresponding to the status */
    std::string error_page(int status_code);

    /* Output when file is deleted */
    std::string file_deleted();

}; // namespace generate

#endif