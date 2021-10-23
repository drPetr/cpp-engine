#pragma once
#include "base_math.hpp"
#include <core/assert.hpp>

namespace engine::core::math
{

class vec3
{
public:
    typedef float   type;

public:
                    vec3() {}
                    vec3( const type x, const type y, const type z ) : 
                            x{x}, y{y}, z{z} {}

    type            operator[]( int index ) const;
    type &          operator[]( int index );
    vec3            operator-() const;
    vec3            operator+( const vec3 &a ) const;
    vec3            operator-( const vec3 &a ) const;
    vec3            operator*( const type a ) const;
    type            operator*( const vec3 &a ) const;
    vec3            operator/( const type a ) const;
    vec3 &          operator+=( const vec3 &a );
    vec3 &          operator-=( const vec3 &a );
    vec3 &          operator*=( const type a );
    vec3 &          operator/=( const vec3 &a );
    vec3 &          operator/=( const type a );
    
    vec3            cross( const vec3 &a ) const;
    vec3 &          cross( const vec3 &a, const vec3 &b );
    type            normalize();
    void            zero();

    const type *    get_ptr() const;
    type *          get_ptr();

public:
    type            x, y, z;
}; /* class vec3 */

extern const vec3 VEC3_ZERO;
extern const vec3 VEC3_ONE;



/* vec3::operator[] */
inline vec3::type vec3::operator[]( int index ) const
{
    math_asserta( index >= 0 && index < 3, "index: %d", index );
    return (&x)[ index ];
}

/* vec3::operator[] */
inline vec3::type &vec3::operator[]( int index )
{
    math_asserta( index >= 0 && index < 3, "index: %d", index );
    return (&x)[ index ];
}

/* vec3::operator- */
inline vec3 vec3::operator-() const
{
    return vec3( -x, -y, -z );
}

/* vec3::operator+ */
inline vec3 vec3::operator+( const vec3 &a ) const
{
    return vec3( x + a.x, y + a.y, z + a.z );
}

/* vec3::operator- */
inline vec3 vec3::operator-( const vec3 &a ) const
{
    return vec3( x - a.x, y - a.y, z - a.z );
}

/* vec3::operator* */
inline vec3::type vec3::operator*( const vec3 &a ) const
{
    return x * a.x + y * a.y + z * a.z;
}

/* vec3::operator* */
inline vec3 vec3::operator*( const vec3::type a ) const
{
    return vec3( x * a, y * a, z * a );
}

/* vec3::operator/ */
inline vec3 vec3::operator/( const vec3::type a ) const
{
    return vec3( x / a, y / a, z / a );
}

/* vec3::operator+= */
inline vec3 &vec3::operator+=( const vec3 &a )
{
    x += a.x;
    y += a.y;
    z += a.z;
    return *this;
}

/* vec3::operator-= */
inline vec3 &vec3::operator-=( const vec3 &a )
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
    return *this;
}

/* vec3::operator*= */
inline vec3 &vec3::operator*=( const vec3::type a )
{
    x *= a;
    y *= a;
    z *= a;
    return *this;
}

/* vec3::operator/= */
inline vec3 &vec3::operator/=( const vec3 &a )
{
    x /= a.x;
    y /= a.y;
    z /= a.z;
    return *this;
}

/* vec3::operator/= */
inline vec3 &vec3::operator/=( const vec3::type a )
{
    x /= a;
    y /= a;
    z /= a;
    return *this;
}

/* vec3::cross */
inline vec3 vec3::cross( const vec3 &a ) const
{
    return vec3( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x );
}

/* vec3::cross */
inline vec3 &vec3::cross( const vec3 &a, const vec3 &b )
{
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
    return *this;
}

/* vec3::normalize */
inline vec3::type vec3::normalize()
{
    auto sq = x * x + y * y + z * z;
    auto inv = engine::core::math::sqrt( 1.0 / sq );
    x *= inv;
    y *= inv;
    z *= inv;
    return inv * sq;
}

/* vec3::zero */
inline void vec3::zero()
{
    *this = VEC3_ZERO;
}

/* vec3::get_ptr */
inline const vec3::type *vec3::get_ptr() const
{
    return &x;
}

/* vec3::get_ptr */
inline vec3::type *vec3::get_ptr()
{
    return &x;
}

} /* namespcace engine::core::math */
