#include "metadata_manager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

// ================= SAVE METADATA =================
bool MetadataManager::saveMetadata(const std::string &fileId,
                                   const std::vector<ChunkInfo> &chunks,
                                   size_t size)
{
    std::filesystem::create_directories("data/metadata");

    std::ofstream out("data/metadata/" + fileId + ".meta");
    if (!out.is_open())
    {
        std::cerr << "ERROR: Cannot write metadata for file: " << fileId << std::endl;
        return false;
    }

    out << "{\n";
    out << "\"file_id\": \"" << fileId << "\",\n";
    out << "\"size\": " << size << ",\n";
    out << "\"chunks\": [\n";

    for (size_t i = 0; i < chunks.size(); i++)
    {
        out << "{ \"id\": \"" << chunks[i].id
            << "\", \"checksum\": \"" << chunks[i].checksum << "\" }";

        if (i != chunks.size() - 1)
            out << ",";
        out << "\n";
    }

    out << "]\n}";
    return true;
}

// ================= LOAD METADATA =================
std::vector<ChunkInfo> MetadataManager::loadChunks(const std::string &fileId)
{
    std::ifstream in("data/metadata/" + fileId + ".meta");

    std::vector<ChunkInfo> chunks;

    if (!in.is_open())
    {
        std::cerr << "ERROR: Cannot open metadata for file: " << fileId << std::endl;
        return chunks;
    }

    // Read full file content
    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());

    size_t pos = 0;

    while ((pos = content.find("\"id\"", pos)) != std::string::npos)
    {
        ChunkInfo info;

        // Extract ID
        size_t idStart = content.find("\"", pos + 4) + 1;
        size_t idEnd = content.find("\"", idStart);
        info.id = content.substr(idStart, idEnd - idStart);

        // Extract checksum (search AFTER idEnd)
        size_t checksumKey = content.find("\"checksum\"", idEnd);
        size_t chkStart = content.find("\"", checksumKey + 10) + 1;
        size_t chkEnd = content.find("\"", chkStart);
        info.checksum = content.substr(chkStart, chkEnd - chkStart);

        chunks.push_back(info);

        pos = chkEnd; // move forward
    }

    return chunks;
}