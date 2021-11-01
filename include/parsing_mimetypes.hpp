#ifndef PARSING_MIMETYPES
#define PARSING_MIMETYPES

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <vector>

namespace ConfParsing
{
    int parsing_mimetypes(const char*                         filename,
                          std::map<std::string, std::string>& mimetypes);
}

#endif
