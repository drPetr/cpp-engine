#include "filesystem.h"
namespace engine {

/* filesystem::open_read */
ifstream filesystem::open_read( const string &filename ) {
    return ifstream( filename, std::ios::in | std::ios::binary );;
}

/* filesystem::open_write */
ofstream filesystem::open_write( const string &filename ) {
    return ofstream( filename, std::ios::out | std::ios::binary );;
}

} /* namespace engine */