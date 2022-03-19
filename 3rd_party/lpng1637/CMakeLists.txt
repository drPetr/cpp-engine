# sources
file(GLOB_RECURSE lpng_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
file(GLOB_RECURSE lpng_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# new target lpng
add_library(lpng STATIC ${lpng_sources} ${lpng_headers})
target_compile_options(lpng PRIVATE -O3)
target_link_libraries(lpng PUBLIC zlib)
