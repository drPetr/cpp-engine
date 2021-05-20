#pragma once
#include <core/math.h>
namespace engine {

using namespace engine::math;

/* object3d_location
* расположение трёхмерного объекта в пространстве
* позиция, масштаб и вращение объекта 
*/
class object3d_location {
public:
    void            rotate( const quat &q );
    void            move( const vec3 &delta );
    void            scale( const vec3 &scale );

    void            set_rotation( const quat &q );
    const quat      &get_rotation() const;
    void            set_position( const vec3 &pos );
    const vec3      &get_position() const;
    void            set_scale( const vec3 &scale );
    const vec3      &get_scale();

    const mat4      &operator()();
protected:
    mat4            out{MAT4_ZERO};     /* out matrix */
    quat            rot{QUAT_ZERO};     /* object rotation */
    vec3            pos{VEC3_ZERO};     /* object position */
    vec3            scl{1.0, 1.0, 1.0}; /* scale object */
    bool            needUpdate{true};   /* need update matrix */
};

/* object3d_location::rotate */
inline void object3d_location::rotate( const quat &q ) {
    rot *= q;
    needUpdate = true;
}

/* object3d_location::move */
inline void object3d_location::move( const vec3 &delta ) {
    pos += delta;
    needUpdate = true;
}

/* object3d_location::scale */
inline void object3d_location::scale( const vec3 &scale ) {
    scl.x *= scale.x;
    scl.y *= scale.y;
    scl.z *= scale.z;
    needUpdate = true;
}

/* object3d_location::set_rotation */
inline void object3d_location::set_rotation( const quat &q ) {
    rot = q;
    needUpdate = true;
}

/* object3d_location::get_rotation */
inline const quat &object3d_location::get_rotation() const {
    return rot;
}

/* object3d_location::set_position */
inline void object3d_location::set_position( const vec3 &pos ) {
    this->pos = pos;
    needUpdate = true;
}

/* object3d_location::get_position */
inline const vec3 &object3d_location::get_position() const {
    return pos;
}

/* object3d_location::set_scale */
inline void object3d_location::set_scale( const vec3 &scale ) {
    scl = scale;
    needUpdate = true;
}

/* object3d_location::get_scale */
inline const vec3 &object3d_location::get_scale() {
    return scl;
}

} /* namespace engine */