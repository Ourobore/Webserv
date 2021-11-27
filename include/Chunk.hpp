#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <iostream>

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

    // Accessors
    int          chunk_length() const;
    std::string& chunk();
};

#endif