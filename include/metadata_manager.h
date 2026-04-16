#pragma once
#include <string>
#include <vector>

struct ChunkInfo {
    std::string id;
    std::string checksum;
};


class MetadataManager {
public:
    void saveMetadata(const std::string& fileId,
                      const std::vector<ChunkInfo>& chunks,
                      size_t size);

    std::vector<ChunkInfo> loadChunks(const std::string& fileId);
};