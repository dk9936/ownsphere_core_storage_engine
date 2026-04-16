#include "storage_engine.h"
#include "chunk_manager.h"
#include "metadata_manager.h"
#include "checksum.h"
#include "encryption.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

void StorageEngine::deleteFile(const std::string& fileId) {
    MetadataManager metadataManager;
    ChunkManager chunkManager;

    std::vector<ChunkInfo> chunks = metadataManager.loadChunks(fileId);

    // delete chunks
    for (const auto& chunk : chunks) {
        std::string path = "data/chunks/" + chunk.id;
        std::remove(path.c_str());
    }

    // delete metadata
    std::string metaPath = "data/metadata/" + fileId + ".meta";
    std::remove(metaPath.c_str());

    std::cout << "Deleted file: " << fileId << std::endl;
}

std::vector<std::string> StorageEngine::listFiles() {
    std::vector<std::string> files;

    std::string path = "data/metadata/";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();

        // remove ".meta"
        if (filename.size() > 5) {
            files.push_back(filename.substr(0, filename.size() - 5));
        }
    }

    return files;
}
// ======================= READ =======================
void StorageEngine::retrieveFile(const std::string &fileId, const std::string &outputPath)
{
    ChunkManager chunkManager;
    MetadataManager metadataManager;

    std::vector<ChunkInfo> chunks = metadataManager.loadChunks(fileId);

    if (chunks.empty())
    {
        std::cerr << "ERROR: No chunks found for file: " << fileId << std::endl;
        return;
    }

    std::ofstream out(outputPath, std::ios::binary);

    for (const auto &chunk : chunks)
    {
        std::string key = "mysecretkey";

        std::vector<char> encryptedData = chunkManager.readChunk(chunk.id);
        std::vector<char> data = decryptData(encryptedData, key);

        // ✅ Verify checksum
        std::string checksum = computeChecksum(data);

        if (checksum != chunk.checksum)
        {
            std::cerr << "ERROR: Data corruption detected in chunk: " << chunk.id << std::endl;
            return;
        }

        out.write(data.data(), data.size());
    }

    std::cout << "File reconstructed: " << outputPath << std::endl;
}

// ======================= WRITE =======================
void StorageEngine::storeFile(const std::string &filePath, const std::string &fileId)
{
    ChunkManager chunkManager;
    MetadataManager metadataManager;

    std::ifstream in(filePath, std::ios::binary);

    if (!in.is_open())
    {
        std::cerr << "ERROR: Cannot open file: " << filePath << std::endl;
        return;
    }

    const size_t CHUNK_SIZE = 4 * 1024 * 1024;

    int chunkIndex = 0;
    std::vector<ChunkInfo> chunkInfos;
    size_t totalSize = 0;

    while (!in.eof())
    {
        std::vector<char> buffer(CHUNK_SIZE);

        in.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = in.gcount();

        if (bytesRead == 0)
            break;

        buffer.resize(bytesRead);

        std::string chunkId = fileId + "_chunk_" + std::to_string(chunkIndex++);

        // Write chunk
        std::string key = "mysecretkey"; // later configurable

        std::vector<char> encryptedData = encryptData(buffer, key);

        chunkManager.writeChunk(chunkId, encryptedData);

        // checksum must be computed on ORIGINAL data (important)
        std::string checksum = computeChecksum(buffer);



        // Store metadata
        ChunkInfo info;
        info.id = chunkId;
        info.checksum = checksum;

        chunkInfos.push_back(info);
        totalSize += bytesRead;
    }

    // Save metadata
    metadataManager.saveMetadata(fileId, chunkInfos, totalSize);

    std::cout << "Stored file: " << fileId << std::endl;
    std::cout << "Total chunks: " << chunkInfos.size() << std::endl;
    std::cout << "Total size: " << totalSize << " bytes" << std::endl;
}