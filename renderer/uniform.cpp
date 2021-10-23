#include "uniform.h"
#include <core/assert.hpp>
namespace engine {

/* uniform::set */
void uniform::set( const mat4 &m ) {
    assert( is_valid() );
    glUniformMatrix4fv( var, 1, GL_TRUE, m.get_ptr() );
}

/* uniform::set */
void uniform::set( int i ) {
    assert( is_valid() );
    glUniform1i( var, i );
}

} /* namespace engine */