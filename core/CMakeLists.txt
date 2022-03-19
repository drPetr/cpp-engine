# sources
file(GLOB_RECURSE core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/windows/*.cpp
)
file(GLOB_RECURSE core_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/windows/*.hpp
)

# new target core_target
add_library(core_target STATIC ${core_sources} ${core_headers})
target_include_directories(core_target PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_options(core_target PRIVATE -Wall)
target_compile_definitions(core_target PRIVATE DEBUG)
