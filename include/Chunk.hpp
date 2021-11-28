#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <iostream>

/* Object that allow to manipulate and process chunks
   from chunked requests */

class Chunk
{
  private:
    size_t      _chunk_length;
    std::string _chunk;

    Chunk();

  public:
    Chunk(std::string& raw_chunk);
    ~Chunk();

    void append(std::string& raw_chunk);

    // Condition checkers
    static bool creation_possible(std::string& raw_chunk);
    bool        completed() const;
    static bool empty_chunk(std::string& raw_chunk);

    // Accessors
    int          chunk_length() const;
    std::string& chunk();
};

#endif