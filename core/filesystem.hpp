#pragma once
#include <core/types.hpp>
#include <core/vector.hpp>
#include <core/string.hpp>

namespace engine::core {

struct file_contents {
    bool            success;
    string          contents;
};

class filesystem {
public:
    static bool             file_exists( const string &path );

                            /* read file */
    static file_contents    read_contents( const string &path, size_t offset = 0, size_t length = 0 );
                            /* open file for reading */
    static ifstream         open_read( const string &filename );
                            /* open file for writing */
    static ofstream         open_write( const string &filename );
};

} /* namespace engine::core */
