cmake_minimum_required(VERSION 3.16)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Tetris ${SOURCES})

target_include_directories(Tetris PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(Tetris PRIVATE ${SDL2_LIBRARIES} SDL2_ttf)