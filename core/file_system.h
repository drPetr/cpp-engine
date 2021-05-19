#pragma once
#include <core/types.h>
namespace engine {
namespace core {


struct file_content {
    bool        success;
    string      contents;
};

class file_system {
public:
    bool            file_exists( const string &path );
    file_content    read_contents( const string &path, size_t offset = 0, size_t length = 0 );
private:
};

inline file_system  fs;

} /* namespace core */
} /* namespace engine */