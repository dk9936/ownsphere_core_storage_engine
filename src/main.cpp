#include "storage_engine.h"
#include <iostream>
#include <limits>

// ======================= MENU =======================
void showMenu() {
    std::cout << "\n=== OwnSphere Storage Engine ===\n";
    std::cout << "1. Store file\n";
    std::cout << "2. Read file\n";
    std::cout << "3. List files\n";
    std::cout << "4. Delete file\n";
    std::cout << "5. Exit\n";
    std::cout << "Choose option: ";
}

// ======================= PROGRESS BAR =======================
void showProgress(int percent) {
    int width = 40;
    int pos = (percent * width) / 100;

    std::cout << "\r[";
    for (int i = 0; i < width; i++) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << percent << "%";
    std::cout.flush();
}

// ======================= MAIN =======================
int main() {
    StorageEngine engine;
    int choice;

    while (true) {
        showMenu();
        std::cin >> choice;

        // Clear newline from buffer (important for getline)
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // ================= STORE =================
        if (choice == 1) {
            std::string filePath, fileId;

            std::cout << "Enter file path: ";
            std::getline(std::cin, filePath);

            std::cout << "Enter file ID: ";
            std::getline(std::cin, fileId);

            if (engine.storeFile(filePath, fileId, showProgress)) {
                std::cout << "\n✅ File stored\n";
            } else {
                std::cout << "\n❌ Failed to store file\n";
            }
        }

        // ================= READ =================
        else if (choice == 2) {
            std::string fileId, outputPath;

            std::cout << "Enter file ID: ";
            std::getline(std::cin, fileId);

            std::cout << "Enter output path: ";
            std::getline(std::cin, outputPath);

            if (engine.retrieveFile(fileId, outputPath, showProgress)) {
                std::cout << "\n✅ File retrieved\n";
            } else {
                std::cout << "\n❌ Failed to retrieve file\n";
            }
        }

        // ================= LIST =================
        else if (choice == 3) {
            auto files = engine.listFiles();

            if (files.empty()) {
                std::cout << "No files stored.\n";
            } else {
                std::cout << "Stored files:\n";
                for (const auto& f : files) {
                    std::cout << "  - " << f << "\n";
                }
            }
        }

        // ================= DELETE =================
        else if (choice == 4) {
            std::string fileId;

            std::cout << "Enter file ID: ";
            std::getline(std::cin, fileId);

            if (engine.deleteFile(fileId)) {
                std::cout << "✅ File deleted\n";
            } else {
                std::cout << "❌ Failed to delete file\n";
            }
        }

        // ================= EXIT =================
        else if (choice == 5) {
            std::cout << "Exiting...\n";
            break;
        }

        // ================= INVALID =================
        else {
            std::cout << "❌ Invalid option\n";
        }
    }

    return 0;
}