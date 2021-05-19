#include "file_system.h"
#include <windows.h>
namespace engine {
namespace core {


/* file_system::file_exists */
bool file_system::file_exists( const string &path ) {
    auto attr = GetFileAttributesA( path.c_str() );
    if( attr != INVALID_FILE_ATTRIBUTES ) {
        return static_cast<bool>( !(attr & FILE_ATTRIBUTE_DIRECTORY) );
    }
    return false;
}

/* file_system::read_contents */
file_content file_system::read_contents( const string &path, size_t offset, size_t length ) {
    file_content result;
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
    return std::move( result );
}

} /* namespace core */
} /* namespace engine */