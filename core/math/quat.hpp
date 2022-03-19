#pragma once
#include <core/assert.hpp>
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"

namespace engine::core::math
{

class alignas(alignof(float) * 4) quat
{
public:
    typedef float   type;
public:

                    quat() {}
                    quat( const type x, const type y, const type z, const type w ) :
                            x{x}, y{y}, z{z}, w{w} {}
                    quat( const vec3 &v, const type angle );
                    quat( const vec4 &v ) : x{v.x}, y{v.y}, z{v.z}, w{v.w} {}

    type            operator[]( int index ) const;
    type            &operator[]( int index );
    quat            operator-() const;
    quat            operator+( const quat &a ) const;
    quat            operator-( const quat &a ) const;
    quat            operator*( const quat &a ) const;
    vec3            operator*( const vec3 &a ) const;
    quat            operator*( type a ) const;
    quat            &operator+=( const quat &a );
    quat            &operator-=( const quat &a );
    quat            &operator*=( const quat &a );
    quat            &operator*=( type a );
    friend quat     operator*( const type a, const quat &b );
    friend vec3     operator*( const vec3 &a, const quat &b );

    quat            inverse() const;
    type            length() const;
    type            normalize();
    mat4            to_mat4();

    const type      *get_ptr() const;
    type            *get_ptr();
public:
    type            x, y, z, w;
}; /* class quat */

extern const quat QUAT_ZERO;

/* quat::quat */
inline quat::quat( const vec3 &v, const type angle )
{
    auto [sin, cos] = sin_cos(angle / 2);
    auto sqLen( v.x * v.x + v.y * v.y + v.z * v.z );
    if( sqLen != 1.0 ) {
        if( sqLen != 0.0 ) {
            auto inv( sqrt(1.0 / sqLen) * sin );
            x = v.x * inv;
            y = v.y * inv;
            z = v.z * inv;
            w = cos;
        } else {
            x = 0.0;
            y = 0.0;
            z = sin;
            w = cos;
        }
    } else {
        x = v.x * sin;
        y = v.y * sin;
        z = v.z * sin;
        w = cos;
    }
}

/* quat::operator[] */
inline quat::type quat::operator[]( int index ) const
{
    math_asserta( index >= 0 && index < 4, "index: %d", index );
    return (&x)[ index ];
}

/* quat::operator[] */
inline quat::type &quat::operator[]( int index )
{
    math_asserta( index >= 0 && index < 4, "index: %d", index );
    return (&x)[ index ];
}

/* quat::operator- */
inline quat quat::operator-() const
{
    return quat( -x, -y, -z, -w );
}

/* quat::operator+ */
inline quat quat::operator+( const quat &a ) const
{
    return quat( x + a.x, y + a.y, z + a.z, w + a.w );
}

/* quat::operator- */
inline quat quat::operator-( const quat &a ) const
{
    return quat( x - a.x, y - a.y, z - a.z, w - a.w );
}

/* quat::operator* */
inline quat quat::operator*( const quat &a ) const
{
    return quat( w*a.x + x*a.w + y*a.z - z*a.y,
        w*a.y - x*a.z + y*a.w + z*a.x,
        w*a.z + x*a.y - y*a.x + z*a.w,
        w*a.w - x*a.x - y*a.y - z*a.z );
}

/* quat::operator* */
inline vec3 quat::operator*( const vec3 &a ) const
{
    type xxzz = x * x - z * z;
    type wwyy = w * w - y * y;
    type xy2 = x * y * 2.0;
    type xz2 = x * z * 2.0;
    type xw2 = x * w * 2.0;
    type yw2 = y * w * 2.0;
    type yz2 = y * z * 2.0;
    type zw2 = z * w * 2.0;
    return vec3( 
        a.x * (xxzz + wwyy) + a.y * (xy2 + zw2) + a.z * (xz2 - yw2),
        a.x * (xy2 - zw2) + a.y * (y*y+w*w-x*x-z*z) + a.z * (yz2 + xw2),
        a.x * (xz2 + yw2) + a.y * (yz2 - xw2) + a.z * (wwyy - xxzz)
    );
}

/* quat::operator* */
inline quat quat::operator*( quat::type a ) const
{
    return quat( x * a, y * a, z * a, w * a );
}

/* quat::operator+= */
inline quat &quat::operator+=( const quat &a )
{
    x += a.x;
    y += a.y;
    z += a.z;
    w += a.w;
    return *this;
}

/* quat::operator-= */
inline quat &quat::operator-=( const quat &a )
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
    w -= a.w;
    return *this;
}

/* quat::operator*= */
inline quat &quat::operator*=( const quat &a )
{
    *this = *this * a;
    return *this;
}

/* quat::operator*= */
inline quat &quat::operator*=( quat::type a )
{
    x *= a;
    y *= a;
    z *= a;
    w *= a;
    return *this;
}

/* operator* */
inline quat operator*( const quat::type a, const quat &b )
{
    return b * a;
}

/* operator* */
inline vec3 operator*( const vec3 &a, const quat &b )
{
    return b * a;
}

/* quat::inverse */
inline quat quat::inverse() const
{
    return quat( -x, -y, -z, w );
}

/* quat::length */
inline quat::type quat::length() const
{
    return sqrt( x * x + y * y + z * z + w * w );
}

/* quat::normalize */
inline quat::type quat::normalize()
{
    if( auto len = length(); len != 0.0 ) {
        auto inv = 1.0 / len;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return len;
    }
    return 0.0;
}

} /* namespace engine::core::math */
