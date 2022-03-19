#pragma once
#include "vec2.hpp"

namespace engine::core::math
{

class rectangle
{
public:
    typedef float       type;

public:
                    rectangle() : min(0.0, 0.0), max(0.0, 0.0) {}

                    rectangle( const point2d &pmin, const point2d &pmax ) 
                            : min(pmin), max(pmax) {}

                    rectangle( type left, type top, type right, type bottom ) 
                            : min( left, top ), max( right, bottom ) {}

    rectangle       operator-() const;
    rectangle       operator+( const point2d &a ) const;
    rectangle       operator-( const point2d &a ) const;
    rectangle       operator*( const type a ) const;
    rectangle       operator*( const point2d &a ) const;
    rectangle       operator/( const type a ) const;
    rectangle       operator/( const point2d &a ) const;
    rectangle &     operator+=( const point2d &a );
    rectangle &     operator-=( const point2d &a );
    rectangle &     operator*=( const type a );
    rectangle &     operator*=( const point2d &a );
    rectangle &     operator/=( const type a );
    rectangle &     operator/=( const point2d &a );                

                    /* set min(-1.0, -1.0) max(1.0, 1.0) */
    void            identity();

    type &          left();
    type            left() const;
    type &          top();
    type            top() const;
    type &          right();
    type            right() const;
    type &          bottom();
    type            bottom() const;
    type            width() const;
    type            height() const;

    point2d         center() const;
    size2d          size() const;

public:
    point2d         min;
    point2d         max;

}; /* class rectangle */



/* rectangle::operator- */
inline rectangle rectangle::operator-() const
{
    return rectangle( -min, -max );
}

/* rectangle::operator+ */
inline rectangle rectangle::operator+( const point2d &a ) const
{
    return rectangle( min + a, max + a );
}

/* rectangle::operator- */
inline rectangle rectangle::operator-( const point2d &a ) const
{
    return rectangle( min - a, max - a );
}

/* rectangle::operator* */
inline rectangle rectangle::operator*( const rectangle::type a ) const
{
    auto c = center();
    rectangle rect( c + (min - c) * a, c + (max - c) * a );
    return rect;
}

/* rectangle::operator* */
inline rectangle rectangle::operator*( const point2d &a ) const
{
    auto c = center();
    auto mmin = c + (min - c);
    mmin.x *= a.x;
    mmin.y *= a.y;
    auto mmax = c + (max - c);
    mmax.x *= a.x;
    mmax.y *= a.y;
    return rectangle( mmin, mmax );
}

/* rectangle::operator/ */
inline rectangle rectangle::operator/( const type a ) const
{
    return (*this) * (1.0 / a);
}

/* rectangle::operator/ */
inline rectangle rectangle::operator/( const point2d &a ) const
{
    auto p = point2d( 1.0, 1.0 );
    p /= a;
    return (*this) * p;
}

/* rectangle::operator+= */
inline rectangle & rectangle::operator+=( const point2d &a )
{
    min += a;
    max += a;
    return *this;
}

/* rectangle::operator-= */
inline rectangle & rectangle::operator-=( const point2d &a )
{
    min -= a;
    max -= a;
    return *this;
}

/* rectangle::operator*= */
inline rectangle & rectangle::operator*=( const rectangle::type a )
{
    *this = *this * a;
    return *this;
}

/* rectangle::operator*= */
inline rectangle & rectangle::operator*=( const point2d &a )
{
    *this = *this * a;
    return *this;
}

/* rectangle::operator/= */
inline rectangle & rectangle::operator/=( const rectangle::type a )
{
    *this = *this / a;
    return *this;
}

/* rectangle::operator/= */
inline rectangle & rectangle::operator/=( const point2d &a )
{
    *this = *this / a;
    return *this;
}

/* rectangle::identity */
inline void rectangle::identity()
{
    min = point2d(-1.0, -1.0);
    max = point2d(1.0, 1.0);
}

/* rectangle::left */
inline rectangle::type & rectangle::left()
{
    container_assert( max.x >= min.x );
    return min.x;
}

/* rectangle::left */
inline rectangle::type rectangle::left() const
{
    container_assert( max.x >= min.x );
    return min.x;
}

/* rectangle::top */
inline rectangle::type & rectangle::top()
{
    container_assert( max.y >= min.y );
    return min.y;
}

/* rectangle::top */
inline rectangle::type rectangle::top() const
{
    container_assert( max.y >= min.y );
    return min.y;
}

/* rectangle::right */
inline rectangle::type & rectangle::right()
{
    container_assert( max.x >= min.x );
    return max.x;
}

/* rectangle::right */
inline rectangle::type rectangle::right() const
{
    container_assert( max.x >= min.x );
    return max.x;
}

/* rectangle::bottom */
inline rectangle::type & rectangle::bottom()
{
    container_assert( max.y >= min.y );
    return max.y;
}

/* rectangle::bottom */
inline rectangle::type rectangle::bottom() const
{
    container_assert( max.y >= min.y );
    return max.y;
}

/* rectangle::width */
inline rectangle::type rectangle::width() const
{
    container_asserta( max.x >= min.x, "max.x: %f; min.x: %f", max.x, min.x );
    return max.x - min.x;
}

/* rectangle::height */
inline rectangle::type rectangle::height() const
{
    container_asserta( max.y >= min.y, "max.y: %f; min.y: %f", max.y, min.y );
    return max.y - min.y;
}

/* rectangle::center */
inline point2d rectangle::center() const
{
    return (max + min) / 2.0;
}

/* rectangle::size */
inline size2d rectangle::size() const
{
    container_asserta( max.x >= min.x, "max.x: %f; min.x: %f", max.x, min.x );
    container_asserta( max.y >= min.y, "max.y: %f; min.y: %f", max.y, min.y );
    return point2d( max.x - min.x, max.y - min.y );
}

} /* namespace engine::core::math */
