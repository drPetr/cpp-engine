#include "filesystem.hpp"

namespace engine::core
{

/* filesystem::file_exists */
bool filesystem::file_exists( const string &path ) {
    auto attr = GetFileAttributesA( path.c_str() );
    if( attr != INVALID_FILE_ATTRIBUTES ) {
        return static_cast<bool>( !(attr & FILE_ATTRIBUTE_DIRECTORY) );
    }
    return false;
}

/* filesystem::read_contents */
file_contents filesystem::read_contents( const string &path, size_t offset, size_t length ) {
    file_contents result;
    std::ifstream file( path, std::ios::in | std::ios::binary );
    result.success = file.is_open();
    if( result.success ) {
        /* calculate or clamp length of file */
        file.seekg( 0, std::ios::end );
        auto fileSize = static_cast<size_t>( file.tellg() );
        if( offset > fileSize ) {
            offset = fileSize;
        }
        if( length + offset > fileSize || length == 0 ) {
            length = fileSize - offset;
        }
        file.seekg( offset, std::ios::beg );
        /* reserve string size and read contents */
        if( length + offset == fileSize ) {
            result.contents.reserve( length );
            result.contents.assign( std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>() );
        } else {
            result.contents.resize( length );
            file.read( static_cast<char*>(result.contents.data()), length );
        }
    }
    return result;
}

/* filesystem::open_read */
ifstream filesystem::open_read( const string &filename ) {
    return ifstream( filename, std::ios::in | std::ios::binary );;
}

/* filesystem::open_write */
ofstream filesystem::open_write( const string &filename ) {
    return ofstream( filename, std::ios::out | std::ios::binary );;
}

} /* namespace engine::core */
