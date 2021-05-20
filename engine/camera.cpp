#include "camera.h"
namespace engine {
using namespace math;

/* camera::camera */
camera::camera( const vec3 &pos, const vec3 &dir, const vec3 &up ) {
    set_position( pos );
    set_direction( dir );
    set_up( up );
    get_right();
    get_up();
}

/* camera::rotate */
void camera::rotate( const quat &q ) {
    set_direction( q * get_direction() );
    set_up( q * get_up() );
}

/* camera::set_position */
void camera::set_direction( const vec3 &dir ) {
    direction = dir;
    direction.normalize();
}

/* camera::set_up */
void camera::set_up( const vec3 &up ) {
    this->up = up;
    this->up.normalize();
}

/* camera::get_up */
const vec3 &camera::get_up() {
    up = right.cross( direction );
    return up;
}

/* camera::get_right */
const vec3 &camera::get_right() {
    right = direction.cross( up );
    return right;
}

/* camera::operator() */
mat4 &camera::operator()() {
    get_right();
    get_up();
    /* optimization (multiply camera matrix on position matrix) */
    out.x.w = right.x * position.x + right.y * position.y + right.z * position.z;
    out.y.w = up.x * position.x + up.y * position.y + up.z * position.z;
    out.z.w = direction.x * position.x + direction.y * position.y + direction.z * position.z;
    return out;
}

} /* namespace engine */