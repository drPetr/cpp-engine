#include "uniform.h"
#include <core/assert.h>
namespace engine {

/* uniform::set */
void uniform::set( const mat4 &m ) {
    assert( is_valid() );
    glUniformMatrix4fv( var, 1, GL_TRUE, m.get_ptr() );
}

} /* namespace engine */