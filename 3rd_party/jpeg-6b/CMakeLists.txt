# sources
file(GLOB_RECURSE ljpeg_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
file(GLOB_RECURSE ljpeg_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# new target ljpeg
add_library(ljpeg STATIC ${ljpeg_sources} ${ljpeg_headers})
target_compile_options(ljpeg PRIVATE -O3)
