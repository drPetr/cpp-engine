#pragma once
#include <core/types.h>
namespace engine {


struct file_contents {
    bool            success;
    vector<byte>    contents;
};


class filesystem {
public:
                            /* read all file */
    static file_contents    read_contents( const string &filename );
                            /* open file for reading */
    static ifstream         open_read( const string &filename );
                            /* open file for writing */
    static ofstream         open_write( const string &filename );
};

} /* namespace engine */