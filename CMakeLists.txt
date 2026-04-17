
cmake_minimum_required(VERSION 3.10)
project(OwnSphereEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)


add_executable(engine
    src/main.cpp
    src/engine/storage_engine.cpp
    src/storage/chunk_manager.cpp
    src/storage/metadata_manager.cpp
    src/crypto/checksum.cpp
    src/crypto/encryption.cpp
)