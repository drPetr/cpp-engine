#pragma once
#include "vec3.h"
#include <core/assert.h>
namespace engine {
namespace math {

class alignas(alignof(float) * 4) vec4 {
public:
    typedef float   type;
public:
                    vec4() {}
                    vec4( const type x, const type y, const type z, const type w ) : 
                            x{x}, y{y}, z{z}, w{w} {}

    type            operator[]( int index ) const;
    type            &operator[]( int index );

    void            zero();
    vec3            &vec3();

    const type      *get_ptr() const;
    type            *get_ptr();

public:
    type            x, y, z, w;
};

extern const vec4 VEC4_ZERO;



/* vec4::operator[] */
inline vec4::type vec4::operator[]( int index ) const {
    assert(index >= 0 && index < 4);
    return (&x)[ index ];
}

/* vec4::operator[] */
inline vec4::type &vec4::operator[]( int index ) {
    assert(index >= 0 && index < 4);
    return (&x)[ index ];
}

/* vec4::zero */
inline void vec4::zero() {
    *this = VEC4_ZERO;
}

/* vec4::vec3 */
inline vec3 &vec4::vec3() {
    return *(reinterpret_cast<engine::math::vec3*>(&x));
}

/* vec4::get_ptr */
inline const vec4::type *vec4::get_ptr() const {
    return &x;
}

/* vec4::get_ptr */
inline vec4::type *vec4::get_ptr() {
    return &x;
}

} /* namespace math */
} /* namespace engine */