#pragma once
#include "vec4.hpp"
#include "vec3.hpp"
#include <utility>
#include <core/assert.hpp>

namespace engine::core::math
{

class alignas(alignof(float) * 4) mat4
{
public:
    typedef float   type;

public:
                    mat4() {}
    explicit        mat4( const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w )
                            : x{x}, y{y}, z{z}, w{w} {}
    explicit        mat4( const type xx, const type xy, const type xz, const type xw,
                            const type yx, const type yy, const type yz, const type yw,
                            const type zx, const type zy, const type zz, const type zw,
                            const type wx, const type wy, const type wz, const type ww )
                            : x{xx, xy, xz, xw},
                            y{yx, yy, yz, yw},
                            z{zx, zy, zz, zw},
                            w{wx, wy, wz, ww} {}

    const vec4      &operator[]( int index ) const;
    vec4            &operator[]( int index );

    vec4            operator*( const vec4 &a ) const;
    vec3            operator*( const vec3 &a ) const;
    mat4            operator*( const mat4 &a ) const;
    mat4            operator+( const mat4 &a ) const;
    mat4            operator-( const mat4 &a ) const;
    mat4            &operator*=( const mat4 &a );
    mat4            &operator+=( const mat4 &a );
    mat4            &operator-=( const mat4 &a );

    void            zero();
    void            identity();

    const type      *get_ptr() const;
    type            *get_ptr();

    static mat4     scale( const vec3 &vScale );
    static mat4     rotation( const vec3 &vRotation );
    static mat4     translation( const vec3 &vTranslation );
    static mat4     perspective( const type fov, const type aspect, const type near, const type far );

public:
    vec4            x, y, z, w;
}; /* class mat4 */

extern const mat4 MAT4_ZERO;
extern const mat4 MAT4_IDENTITY;



/* mat4::operator[] */
inline const vec4 &mat4::operator[]( int index ) const
{
    math_asserta( index >= 0 && index < 4, "index: %d", index );
    return (&x)[ index ];
}

/* mat4::operator[] */
inline vec4 &mat4::operator[]( int index )
{
    math_asserta( index >= 0 && index < 4, "index: %d", index );
    return (&x)[ index ];
}

/* mat4::zero */
inline void mat4::zero()
{
    *this = MAT4_ZERO;
}

/* mat4::identity */
inline void mat4::identity()
{
    *this = MAT4_IDENTITY;
}

/* mat4::get_ptr */
inline const mat4::type *mat4::get_ptr() const
{
    return &x.x;
}

/* mat4::get_ptr */
inline mat4::type *mat4::get_ptr()
{
    return &x.x;
}

} /* namespace engine::core::math */
