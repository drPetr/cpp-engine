# sources
file(GLOB_RECURSE zlib_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
file(GLOB_RECURSE zlib_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# new target zlib
add_library(zlib STATIC ${zlib_sources} ${zlib_headers})
target_include_directories(zlib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../zlib-1.2.11)
target_compile_options(zlib PRIVATE -O3)
