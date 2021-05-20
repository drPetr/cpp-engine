#pragma once
#include <core/math.h>
namespace engine {
using namespace math;

class camera {
public:
                    camera() {}
                    camera( const vec3 &pos, const vec3 &dir, const vec3 &up );

    void            move( const vec3 &delta );
    void            rotate( const quat &q );

    void            set_position( const vec3 &pos );
    void            set_direction( const vec3 &dir );
    void            set_up( const vec3 &up );

    const vec3      get_position() const;
    const vec3      &get_direction() const;
    const vec3      &get_up();
    const vec3      &get_right();

    mat4            &operator()();
private:
    vec3            position{VEC3_ZERO};    /* camera position */
    mat4            out{MAT4_IDENTITY};     /* output camera matrix */
    vec3            &right{out.x.vec3()};   /* camera right vector */
    vec3            &up{out.y.vec3()};      /* camera up vector */
    vec3            &direction{out.z.vec3()};/* camera direction */
};

/* camera::move */
inline void camera::move( const vec3 &delta ) {
    position -= delta;
}

/* camera::set_position */
inline void camera::set_position( const vec3 &pos ) {
    position = -pos;
}

/* camera::get_position */
inline const vec3 camera::get_position() const {
    return -position;
}

/* camera::get_direction */
inline const vec3 &camera::get_direction() const {
    return direction;
}

} /* namespace engine */