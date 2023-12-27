cmake_minimum_required(VERSION 3.12)

project(YetAnotherDoomClone)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)


find_package(SFML COMPONENTS graphics window system REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2IMAGE_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(yadooc ${SOURCES})

target_include_directories(yadooc PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(yadooc sfml-graphics sfml-window sfml-system)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

