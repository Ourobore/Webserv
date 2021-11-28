#include "Chunk.hpp"
#include "utilities.hpp"

Chunk::Chunk(std::string& raw_chunk)
{
    size_t crlf_pos = raw_chunk.find("\r\n");
    _chunk_length = ft::to_dec(raw_chunk.substr(0, crlf_pos));

    // We append _chunk_length if we have it, else just what we have
    size_t substr_length =
        (raw_chunk.substr(crlf_pos + 2).length() < _chunk_length)
            ? raw_chunk.substr(crlf_pos + 2).length()
            : _chunk_length;

    _chunk.append(raw_chunk.substr(crlf_pos + 2, substr_length));

    // If we have all the chunk, cut everything to next chunk
    if (substr_length == _chunk_length)
        raw_chunk = raw_chunk.substr(crlf_pos + 2 + substr_length + 2);
    else
        raw_chunk = raw_chunk.substr(crlf_pos + 2 + substr_length);
}

Chunk::~Chunk()
{
}

void Chunk::append(std::string& raw_chunk)
{
    // If we don't have more than this chunk
    if (raw_chunk.length() + _chunk.length() < _chunk_length)
    {
        _chunk.append(raw_chunk);
        raw_chunk.clear();
    }
    else
    {
        // Else we have to append what is ours, and cut the rest
        size_t      end_pos = _chunk_length - _chunk.length();
        std::string end_chunk = raw_chunk.substr(0, end_pos);
        _chunk.append(end_chunk);
        raw_chunk = raw_chunk.substr(end_pos + 2);
    }
}

// Condition checkers
bool Chunk::creation_possible(std::string& raw_chunk)
{
    // We can create a chunk only if we have it's size
    size_t crlf_pos = raw_chunk.find("\r\n");

    if (crlf_pos == std::string::npos || empty_chunk(raw_chunk))
        return (false);
    else
        return (true);
}

bool Chunk::completed() const
{
    // Completed if our real chunk length is equal to the one we were given
    if (_chunk_length == _chunk.length())
        return (true);
    else
        return (false);
}

bool Chunk::empty_chunk(std::string& raw_chunk)
{
    if (raw_chunk == "0\r\n\r\n")
        return (true);
    else
        return (false);
}

// Accessors
int Chunk::chunk_length() const
{
    return (_chunk_length);
}

std::string& Chunk::chunk()
{
    return (_chunk);
}