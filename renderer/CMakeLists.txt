# sources
file(GLOB_RECURSE renderer_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/*.cpp
)
file(GLOB_RECURSE renderer_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/*.h
)

# new target renderer_target
add_library(renderer_target STATIC ${renderer_sources} ${renderer_headers})
target_link_libraries(renderer_target PUBLIC core_target)
target_link_libraries(renderer_target PUBLIC ljpeg)
target_link_libraries(renderer_target PUBLIC lpng)
target_include_directories(renderer_target PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(renderer_target PRIVATE $CACHE{third_party_dir})
target_compile_options(renderer_target PRIVATE -Wall)
target_compile_definitions(renderer_target PRIVATE DEBUG)
