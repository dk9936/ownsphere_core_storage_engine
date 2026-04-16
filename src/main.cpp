#include "storage_engine.h"
#include <iostream>

int main(int argc, char* argv[]) {
    StorageEngine engine;

    if (argc < 2) {
        std::cout << "Usage:\n";
        std::cout << "  store <filePath> <fileId>\n";
        std::cout << "  read <fileId> <outputPath>\n";
        std::cout << "  list\n";
        std::cout << "  delete <fileId>\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "store" && argc == 4) {
        engine.storeFile(argv[2], argv[3]);
    } 
    else if (command == "read" && argc == 4) {
        engine.retrieveFile(argv[2], argv[3]);
    } 
    else if (command == "list") {
        auto files = engine.listFiles();
        for (const auto& f : files) {
            std::cout << f << std::endl;
        }
    } 
    else if (command == "delete" && argc == 3) {
        engine.deleteFile(argv[2]);
    } 
    else {
        std::cout << "Invalid command\n";
    }

    return 0;
}