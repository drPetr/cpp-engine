#include "camera.h"
namespace engine {
using namespace math;

const mat4 MAT4_CAMERA_INITIAL { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

/* camera::camera */
camera::camera( const vec3 &pos, const vec3 &dir, const vec3 &up ) {
    set_position( pos );
    set_direction( dir );
    set_up( up );
}

/* camera::rotate */
void camera::rotate( const quat &q ) {
    set_direction( q * get_direction() );
    set_up( q * get_up() );
}

/* camera::set_orientation */
void camera::set_orientation( const vec3 &dir, const vec3 &up ) {
    this->dir = dir;
    this->dir.normalize();
    this->up = up;
    this->up.normalize();
    this->right = this->dir.cross( this->up );
    this->up = this->right.cross( this->dir );
}

/* camera::set_direction */
void camera::set_direction( const vec3 &dir ) {
    this->dir = dir;
    this->dir.normalize();
    /* update right */
    right = this->dir.cross( up );
    /* update up */
    up = right.cross( this->dir );
}

/* camera::set_up */
void camera::set_up( const vec3 &up ) {
    this->up = up;
    this->up.normalize();
    /* update right */
    right = dir.cross( this->up );
    /* update up */
    this->up = right.cross( dir );
}

/* camera::get_scale */
mat4 camera::operator()() {
    mat4 out(mat);
    /* scaling */
    out.x.x *= scale.x;
    out.y.y *= scale.y;
    out.z.z *= scale.z;
    /* optimization (multiply camera matrix on position matrix) */
    out.x.w = right.x * pos.x + right.y * pos.y + right.z * pos.z;
    out.y.w = up.x * pos.x + up.y * pos.y + up.z * pos.z;
    out.z.w = dir.x * pos.x + dir.y * pos.y + dir.z * pos.z;
    return std::move(out);
}

} /* namespace engine */