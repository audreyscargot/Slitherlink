cmake_minimum_required(VERSION 4.3)
project(Slitherlink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(Slitherlink main.cpp
        Grid.cpp
        Grid.h
        Data.cpp
        Data.h
        Solver.cpp
        Solver.h
        Tests.cpp
        Tests.h
)

target_link_libraries(Slitherlink PRIVATE raylib)

file(COPY ${CMAKE_SOURCE_DIR}/puzzles DESTINATION ${CMAKE_BINARY_DIR})