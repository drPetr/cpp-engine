#pragma once
#include <renderer/opengl/gl.h>
#include <core/math.hpp>
namespace engine {
using namespace engine::core::math;

class uniform {
public:
                        uniform( GLint var ) : var{var} {}
    void                set( const mat4 &m );
    void                set( int i );
    bool                is_valid();
private:
    GLint               var{-1};
};

/* uniform::is_valid */
inline bool uniform::is_valid() {
    return var != -1;
}

} /* namespace engine */