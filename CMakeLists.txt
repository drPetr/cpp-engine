cmake_minimum_required(VERSION 3.9)

project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MAKE_PROGRAM "make")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/3rd_party)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/renderer)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/engine)

add_executable(_project WIN32 main.cpp)

target_link_libraries(_project PRIVATE -lOpengl32 -lglu32 -lGdi32 -lUser32)
target_link_libraries(_project PUBLIC core_target engine_target renderer_target)
target_link_options(_project PRIVATE -mwindows)
target_include_directories(_project PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(_project PRIVATE -Wall)
target_compile_definitions(_project PRIVATE DEBUG)
