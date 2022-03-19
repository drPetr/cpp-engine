#pragma once
#include <cmath>
#include <core/assert.hpp>

namespace engine::core::math
{

struct sincos_t
{
    sincos_t() = default;
    sincos_t( float s, float c ) : sin{s}, cos{c} {}
    float sin;
    float cos;
};

inline const float pi = 3.14159265358979323846;

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

template <class T> T    min( T &x, T &y, T &z );
template <class T> T    min( T &x, T &y );
template <class T> T    max( T &x, T &y, T &z );
template <class T> T    max( T &x, T &y );
template <class T> T    sign( T a );
template <class T> T    squr( T a );


inline float            abs( float a );
inline int              abs ( int a );
inline long long int    abs( long long int a );
inline float            sqrt( float a );
inline float            sin( float a );
inline float            cos( float a );
inline sincos_t         sin_cos( float a );
inline float            tan( float a );
inline float            asin( float a );
inline float            acos( float a );
inline float            atan( float a );
inline float            atan2( float x, float y );
inline float            log( float a );
inline float            exp( float a );
inline float            pow( float base, float power );
inline float            deg2rad( float a );
inline float            rad2deg( float a );
inline float            floor( float a );
inline float            ceil( float a );
inline float            frac( float a );
inline float            round( float a );
inline float            clamp( float val, float min, float max );
inline int              clamp( int val, int min, int max );


/* min */
template <class T>
T min( T &x, T &y, T &z )
{
    return (x < y) ? ( (x < z) ? x : z ) : ( (y < z) ? y : z );
}

/* min */
template <class T>
T min( T &x, T &y )
{
    return x < y ? x : y;
}

/* max */
template <class T>
T max( T &x, T &y, T &z )
{ 
    return (x > y) ? ( (x > z) ? x : z ) : ( (y > z) ? y : z );
}

/* max */
template <class T>
T max( T &x, T &y )
{ 
    return x > y ? x : y;
}

/* sign */
template <class T>
T sign( T a )
{
    return (a > 0) ? 1 : ( (a < 0) ? -1 : 0 ); 
}

/* squr */
template <class T>
T squr( T a )
{
    return a * a;
}

/* abs */
inline float abs( float a )
{
    void *ptr = reinterpret_cast<void*>(&a);
    unsigned int bits = (*reinterpret_cast<unsigned int*>(ptr));
    bits &= 0x7fffffff;
    ptr = reinterpret_cast<void*>(&bits);
    return *reinterpret_cast<float*>(ptr);
}

/* abs 
* http://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs */
inline int abs ( int a )
{
    auto mask = a >> (sizeof(int) * 8 - 1);
    return (a + mask) ^ mask;
}

/* abs */
inline long long int abs( long long int a )
{
    auto mask = a >> (sizeof(long long int) * 8 - 1);
    return (a + mask) ^ mask;
}

/* sqrt */
inline float sqrt( float a )
{
    return sqrtf(a);
}

/* sin */
inline float sin( float a )
{
    return sinf(a);
}

/* cos */
inline float cos( float a )
{
    return cosf(a);
}

/* sin_cos */
inline sincos_t sin_cos( float a )
{
    return {sin(a), cos(a)};
}

/* tan */
inline float tan( float a )
{
    return tanf(a);
}

/* asin */
inline float asin( float a )
{
    return asinf(a);
}

/* acos */
inline float acos( float a )
{
    return acosf(a);
}

/* atan */
inline float atan( float a )
{
    return atanf(a);
}

/* atan2 */
inline float atan2( float x, float y )
{
    return atan2f(x, y);
}

/* log */
inline float log( float a )
{
    return logf(a);
}

/* exp */
inline float exp( float a )
{
    return expf(a);
}

/* pow */
inline float pow( float base, float power )
{
    return powf(base, power);
}

/* deg2rad */
inline float deg2rad( float a )
{
    return a * pi / 180.0;
}

/* rad2deg */
inline float rad2deg( float a )
{
    return a * 180.0 / pi;
}

/* floor */
inline float floor( float a )
{
    return floorf(a);
}

/* ceil */
inline float ceil( float a )
{
    return ceilf(a);
}

/* frac */
inline float frac( float a )
{
    return a - floorf(a);
}

/* round */
inline float round( float a )
{
    return floorf(a + 0.5);
}

/* clamp */
inline float clamp( float val, float min, float max )
{
    math_asserta( min <= max, "min: %f; max: %f", min, max );
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

/* clamp */
inline int clamp( int val, int min, int max )
{
    math_asserta( min <= max, "min: %d; max: %d", min, max );
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

} /* namespace engine::core::math */
