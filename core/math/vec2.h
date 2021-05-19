#pragma once
#include "base_math.h"
#include <core/assert.h>
namespace engine {
namespace math {

class vec2 {
public:
    typedef float   type;
public:
                    vec2() {}
                    vec2( const type x, const type y ) : x{x}, y{y} {}

    type            operator[]( int index ) const;
    type            &operator[]( int index );
    vec2            operator-() const;
    vec2            operator+( const vec2 &a ) const;
    vec2            operator-( const vec2 &a ) const;
    vec2            operator*( const type a ) const;
    type            operator*( const vec2 &a ) const;
    vec2            operator/( const type a ) const;
    vec2            &operator+=( const vec2 &a );
    vec2            &operator-=( const vec2 &a );
    vec2            &operator*=( const type a );
    vec2            &operator/=( const vec2 &a );
    vec2            &operator/=( const type a );
    
    type            normalize();
    void            zero();

    const type      *get_ptr() const;
    type            *get_ptr();

public:
    type            x, y;
};

extern const vec2 VEC2_ZERO;


/* vec2::operator[] */
inline vec2::type vec2::operator[]( int index ) const {
    assert(index >= 0 && index < 2);
    return (&x)[ index ];
}

/* vec2::operator[] */
inline vec2::type &vec2::operator[]( int index ) {
    assert(index >= 0 && index < 2);
    return (&x)[ index ];
}

/* vec2::operator- */
inline vec2 vec2::operator-() const {
    return vec2( -x, -y );
}

/* vec2::operator+ */
inline vec2 vec2::operator+( const vec2 &a ) const {
    return vec2( x + a.x, y + a.y );
}

/* vec2::operator- */
inline vec2 vec2::operator-( const vec2 &a ) const {
    return vec2( x - a.x, y - a.y );
}

/* vec2::operator* */
inline vec2::type vec2::operator*( const vec2 &a ) const {
    return x * a.x + y * a.y;
}

/* vec2::operator* */
inline vec2 vec2::operator*( const vec2::type a ) const {
    return vec2( x * a, y * a );
}

/* vec2::operator/ */
inline vec2 vec2::operator/( const vec2::type a ) const {
    return vec2( x / a, y / a );
}

/* vec2::operator+= */
inline vec2 &vec2::operator+=( const vec2 &a ) {
    x += a.x;
    y += a.y;
    return *this;
}

/* vec2::operator-= */
inline vec2 &vec2::operator-=( const vec2 &a ) {
    x -= a.x;
    y -= a.y;
    return *this;
}

/* vec2::operator*= */
inline vec2 &vec2::operator*=( const vec2::type a ) {
    x *= a;
    y *= a;
    return *this;
}

/* vec2::operator/= */
inline vec2 &vec2::operator/=( const vec2 &a ) {
    x /= a.x;
    y /= a.y;
    return *this;
}

/* vec2::operator/= */
inline vec2 &vec2::operator/=( const vec2::type a ) {
    x /= a;
    y /= a;
    return *this;
}

/* vec2::normalize */
inline vec2::type vec2::normalize() {
    auto sq = x * x + y * y;
    auto inv = sqrt( 1.0 / sq );
    x *= inv;
    y *= inv;
    return inv * sq;
}

/* vec2::zero */
inline void vec2::zero() {
    *this = VEC2_ZERO;
}

/* vec2::get_ptr */
inline const vec2::type *vec2::get_ptr() const {
    return &x;
}

/* vec2::get_ptr */
inline vec2::type *vec2::get_ptr() {
    return &x;
}

} /* namespace math */
} /* namespace engine */