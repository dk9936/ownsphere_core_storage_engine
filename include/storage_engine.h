#pragma once
#include <string>
#include <vector>  

// ✅ ADD THIS
class StorageEngine
{
public:
    std::vector<std::string> listFiles();
    void deleteFile(const std::string &fileId);
    void storeFile(const std::string &filePath, const std::string &fileId);
    void retrieveFile(const std::string &fileId, const std::string &outputPath);
};