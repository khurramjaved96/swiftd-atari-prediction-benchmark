cmake_minimum_required(VERSION 3.26)
project(swifttd-atari)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-O3)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

add_executable(SwiftTDOnAtari main.cpp src/utils.cpp)
add_executable(Count count.cpp)
add_subdirectory(src)

# link libraries list
set(link_libraries_list)
list(APPEND link_libraries_list Exp)
list(APPEND link_libraries_list Atari)
list(APPEND link_libraries_list SwiftTD)
list(APPEND link_libraries_list Threads::Threads)
list(APPEND link_libraries_list ZLIB::ZLIB)
list(APPEND link_libraries_list pthread)
list(APPEND link_libraries_list Evaluators)
list(APPEND link_libraries_list Logger)

target_link_libraries(SwiftTDOnAtari PRIVATE ${link_libraries_list})
target_link_libraries(Count PRIVATE Exp)


