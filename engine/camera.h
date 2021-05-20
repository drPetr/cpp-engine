#pragma once
#include <core/math.h>
namespace engine {
using namespace math;

extern const mat4 MAT4_CAMERA_INITIAL;

class camera {
public:
                    camera() {}
                    camera( const vec3 &pos, const vec3 &dir, const vec3 &up );
    void            move( const vec3 &delta );
    void            rotate( const quat &q );
    void            set_orientation( const vec3 &dir, const vec3 &up );
    void            set_direction( const vec3 &dir );
    const vec3      &get_direction() const;
    void            set_up( const vec3 &up );
    const vec3      &get_up() const;
    const vec3      &get_right() const;
    void            set_position( const vec3 &pos );
    const vec3      get_position() const;
    void            set_scale( const vec3 &scale );
    const vec3      &get_scale();

    mat4            operator()();
private:
    mat4            mat{MAT4_CAMERA_INITIAL}; /* transform matrix */
    vec3            &right{mat.x.vec3()};   /* camera right vector */
    vec3            &up{mat.y.vec3()};      /* right normal vector */
    vec3            &dir{mat.z.vec3()};     /* camera direction */
    vec3            pos{VEC3_ZERO};         /* camera position */
    vec3            scale{1.0, 1.0, 1.0};   /* scale camera */
};

/* camera::move */
inline void camera::move( const vec3 &delta ) {
    pos -= delta;
}

/* camera::get_direction */
inline const vec3 &camera::get_direction() const {
    return dir;
}

/* camera::get_up */
inline const vec3 &camera::get_up() const {
    return up;
}

/* camera::get_right */
inline const vec3 &camera::get_right() const {
    return right;
}

/* camera::set_position */
inline void camera::set_position( const vec3 &pos ) {
    this->pos = -pos;
}

/* camera::get_position */
inline const vec3 camera::get_position() const {
    return -pos;
}

/* camera::set_scale */
inline void camera::set_scale( const vec3 &scale ) {
    this->scale = scale;
}

/* camera::get_scale */
inline const vec3 &camera::get_scale() {
    return scale;
}

} /* namespace engine */