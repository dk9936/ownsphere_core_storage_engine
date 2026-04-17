#include "storage_engine.h"
#include "chunk_manager.h"
#include "metadata_manager.h"
#include "checksum.h"
#include "encryption.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <mutex>

static std::unordered_map<std::string, int> progressMap;
static std::mutex progressMutex;

// ======================= PROGRESS =======================
static void updateProgress(const std::string& fileId, int percent) {
    std::lock_guard<std::mutex> lock(progressMutex);
    progressMap[fileId] = percent;
}

// ======================= GET PROGRESS =======================
int StorageEngine::getProgress(const std::string& fileId) {
    std::lock_guard<std::mutex> lock(progressMutex);
    if (progressMap.find(fileId) == progressMap.end()) return 0;
    return progressMap[fileId];
}

// ======================= WRITE =======================
bool StorageEngine::storeFile(const std::string &filePath,
                              const std::string &fileId,
                              ProgressCallback progressCallback)
{
    ChunkManager chunkManager;
    MetadataManager metadataManager;

    std::ifstream in(filePath, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "ERROR: Cannot open file: " << filePath << std::endl;
        return false;
    }

    // Get file size
    in.seekg(0, std::ios::end);
    size_t fileSize = in.tellg();
    in.seekg(0);

    const size_t CHUNK_SIZE = 4 * 1024 * 1024;

    size_t processed = 0;
    int chunkIndex = 0;
    std::vector<ChunkInfo> chunkInfos;

    while (true) {
        std::vector<char> buffer(CHUNK_SIZE);

        in.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = in.gcount();

        if (bytesRead <= 0) break;

        buffer.resize(bytesRead);

        std::string chunkId = fileId + "_chunk_" + std::to_string(chunkIndex++);

        std::string key = "mysecretkey";
        std::vector<char> encryptedData = encryptData(buffer, key);

        if (!chunkManager.writeChunk(chunkId, encryptedData)) {
            return false;
        }

        std::string checksum = computeChecksum(buffer);

        ChunkInfo info;
        info.id = chunkId;
        info.checksum = checksum;

        chunkInfos.push_back(info);

        // ===== Progress update =====
        processed += bytesRead;
        int percent = static_cast<int>((processed * 100) / fileSize);

        updateProgress(fileId, percent);

        if (progressCallback) {
            progressCallback(percent);
        }
    }

    if (!metadataManager.saveMetadata(fileId, chunkInfos, fileSize)) {
        return false;
    }

    updateProgress(fileId, 100);

    std::cout << "\nStored file: " << fileId << std::endl;
    return true;
}

// ======================= READ =======================
bool StorageEngine::retrieveFile(const std::string &fileId,
                                 const std::string &outputPath,
                                 ProgressCallback progressCallback)
{
    ChunkManager chunkManager;
    MetadataManager metadataManager;

    auto chunks = metadataManager.loadChunks(fileId);

    if (chunks.empty()) {
        std::cerr << "ERROR: No chunks found\n";
        return false;
    }

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "ERROR: Cannot open output file\n";
        return false;
    }

    int totalChunks = chunks.size();
    int processedChunks = 0;

    for (const auto& chunk : chunks) {
        std::string key = "mysecretkey";

        auto encryptedData = chunkManager.readChunk(chunk.id);
        if (encryptedData.empty()) return false;

        auto data = decryptData(encryptedData, key);

        if (computeChecksum(data) != chunk.checksum) {
            std::cerr << "ERROR: Data corruption\n";
            return false;
        }

        out.write(data.data(), data.size());

        // ===== Progress update =====
        processedChunks++;
        int percent = (processedChunks * 100) / totalChunks;

        updateProgress(fileId, percent);

        if (progressCallback) {
            progressCallback(percent);
        }
    }

    std::cout << "\nFile reconstructed: " << outputPath << std::endl;
    return true;
}

// ======================= DELETE =======================
bool StorageEngine::deleteFile(const std::string& fileId) {
    MetadataManager metadataManager;
    ChunkManager chunkManager;

    std::vector<ChunkInfo> chunks = metadataManager.loadChunks(fileId);
    if (chunks.empty()) {
        std::cerr << "ERROR: No chunks found for file: " << fileId << std::endl;
        return false;
    }

    // delete chunks
    for (const auto& chunk : chunks) {
        std::string path = "data/chunks/" + chunk.id;
        std::remove(path.c_str());
    }

    // delete metadata
    std::string metaPath = "data/metadata/" + fileId + ".meta";
    std::remove(metaPath.c_str());

    std::cout << "Deleted file: " << fileId << std::endl;
    return true;
}

// ======================= LIST =======================
std::vector<std::string> StorageEngine::listFiles() {
    std::vector<std::string> files;
    std::string path = "data/metadata/";

    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::string filename = entry.path().filename().string();

            if (filename.size() > 5) { // remove ".meta"
                files.push_back(filename.substr(0, filename.size() - 5));
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "ERROR: Cannot list files: " << e.what() << std::endl;
    }

    return files;
}