#include "chunk_manager.h"
#include <fstream>

void ChunkManager::writeChunk(const std::string& chunkId, const std::vector<char>& data) {
    std::ofstream out("data/chunks/" + chunkId, std::ios::binary);
    out.write(data.data(), data.size());
}

std::vector<char> ChunkManager::readChunk(const std::string& chunkId) {
    std::ifstream in("data/chunks/" + chunkId, std::ios::binary);

    return std::vector<char>((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
}