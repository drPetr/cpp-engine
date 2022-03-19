# sources
file(GLOB_RECURSE engine_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
file(GLOB_RECURSE engine_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# new target engine_target
add_library(engine_target STATIC ${engine_sources} ${engine_headers})
target_link_libraries(engine_target PUBLIC core_target)
target_link_libraries(engine_target PUBLIC renderer_target)
target_include_directories(engine_target PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(engine_target PRIVATE $CACHE{third_party_dir})
target_compile_options(engine_target PRIVATE -Wall)
target_compile_definitions(engine_target PRIVATE DEBUG)
