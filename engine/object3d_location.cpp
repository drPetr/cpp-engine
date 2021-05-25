#include "object3d_location.h"
namespace engine {

const mat4 &object3d_location::operator()() {
    if( needUpdate ) {
        out = mat4::translation(pos) ;
        /* scaling */
        out.x.x *= scl.x;
        out.y.y *= scl.y;
        out.z.z *= scl.z;
        /* rotation */
        out *= rot.to_mat4();
        needUpdate = false;
    }
    return out;
}

} /* namespace engine */