#pragma once
#include <vector>
#include <string>

class ChunkManager {
public:
    void writeChunk(const std::string& chunkId, const std::vector<char>& data);
    std::vector<char> readChunk(const std::string& chunkId);
};