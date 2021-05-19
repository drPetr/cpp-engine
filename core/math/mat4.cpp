#include "mat4.h"
#include "base_math.h"
namespace engine {
namespace math {

const mat4 MAT4_ZERO { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
const mat4 MAT4_IDENTITY { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

/* mat4::operator* */
vec4 mat4::operator*( const vec4 &a ) const {
    return vec4(
        x.x * a.x + x.y * a.y + x.z * a.z + x.w * a.w,
        y.x * a.x + y.y * a.y + y.z * a.z + y.w * a.w,
        z.x * a.x + z.y * a.y + z.z * a.z + z.w * a.w,
        w.x * a.x + w.y * a.y + w.z * a.z + w.w * a.w
    );
}

/* mat4::operator* */
vec3 mat4::operator*( const vec3 &a ) const {
    auto scale = w.x * a.x + w.y * a.y + w.z * a.z + w.w;
    if( scale == 1.0 ) {
        return vec3(
            x.x * a.x + x.y * a.y + x.z * a.z + x.w,
            y.x * a.x + y.y * a.y + y.z * a.z + y.w,
            z.x * a.x + z.y * a.y + z.z * a.z + z.w
        );
    } else if( scale == 0.0 ) {
        return vec3( 0.0, 0.0, 0.0 );
    } else {
        auto invScale = 1.0 / scale;
        return vec3(
            (x.x * a.x + x.y * a.y + x.z * a.z + x.w) * invScale,
            (y.x * a.x + y.y * a.y + y.z * a.z + y.w) * invScale,
            (z.x * a.x + z.y * a.y + z.z * a.z + z.w) * invScale
        );
    }
}

/* mat4::operator* */
mat4 mat4::operator*( const mat4 &a ) const {
    mat4 mat;
    auto l = reinterpret_cast<const float*>(this);
    auto r = reinterpret_cast<const float*>(&a);
    auto m = reinterpret_cast<float*>(&mat);
    for( auto i = 0; i < 4; i++ ) {
        for( auto j = 0; j < 4; j++ ) {
            *m =  l[0] * r[0 * 4 + j]
                + l[1] * r[1 * 4 + j]
                + l[2] * r[2 * 4 + j]
                + l[3] * r[3 * 4 + j];
            m++;
        }
        l += 4;
    }
    return std::move(mat);
}

mat4 mat4::operator+( const mat4 &a ) const {
    return mat4(
        x.x + a.x.x, x.y + a.x.y, x.z + a.x.z, x.w + a.x.w,
        y.x + a.y.x, y.y + a.y.y, y.z + a.y.z, y.w + a.y.w,
        z.x + a.z.x, z.y + a.z.y, z.z + a.z.z, z.w + a.z.w,
        w.x + a.w.x, w.y + a.w.y, w.z + a.w.z, w.w + a.w.w
    );
}

mat4 mat4::operator-( const mat4 &a ) const {
    return mat4(
        x.x - a.x.x, x.y - a.x.y, x.z - a.x.z, x.w - a.x.w,
        y.x - a.y.x, y.y - a.y.y, y.z - a.y.z, y.w - a.y.w,
        z.x - a.z.x, z.y - a.z.y, z.z - a.z.z, z.w - a.z.w,
        w.x - a.w.x, w.y - a.w.y, w.z - a.w.z, w.w - a.w.w
    );
}

mat4 &mat4::operator*=( const mat4 &a ) {
    *this = (*this) * a;
    return *this;
}

/* mat4::operator+= */
mat4 &mat4::operator+=( const mat4 &a ) {
    x.x += a.x.x;
    x.y += a.x.y;
    x.z += a.x.z;
    x.w += a.x.w;
    y.x += a.y.x;
    y.y += a.y.y;
    y.z += a.y.z;
    y.w += a.y.w;
    z.x += a.z.x;
    z.y += a.z.y;
    z.z += a.z.z;
    z.w += a.z.w;
    w.x += a.w.x;
    w.y += a.w.y;
    w.z += a.w.z;
    w.w += a.w.w;
    return *this;
}

/* mat4::operator-= */
mat4 &mat4::operator-=( const mat4 &a ) {
    x.x -= a.x.x;
    x.y -= a.x.y;
    x.z -= a.x.z;
    x.w -= a.x.w;
    y.x -= a.y.x;
    y.y -= a.y.y;
    y.z -= a.y.z;
    y.w -= a.y.w;
    z.x -= a.z.x;
    z.y -= a.z.y;
    z.z -= a.z.z;
    z.w -= a.z.w;
    w.x -= a.w.x;
    w.y -= a.w.y;
    w.z -= a.w.z;
    w.w -= a.w.w;
    return *this;
}

/* mat4::scale */
mat4 mat4::scale( const vec3 &vScale ) {
    return mat4(
        vScale.x, 0.0, 0.0, 0.0,
        0.0, vScale.y, 0.0, 0.0,
        0.0, 0.0, vScale.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

/* mat4::rotation */
mat4 mat4::rotation( const vec3 &vRotation ) {
    mat4 out;
    out.identity();
    if( vRotation.x != 0.0 ) {
        auto [sin, cos] = engine::math::sin_cos( vRotation.x );
        mat4 rx{
            1.0, 0.0, 0.0, 0.0,
            0.0, cos,-sin, 0.0,
            0.0, sin, cos, 0.0,
            0.0, 0.0, 0.0, 1.0
        };
        out *= rx;
    }
    if( vRotation.y != 0.0 ) {
        auto [sin, cos] = engine::math::sin_cos( vRotation.y );
        mat4 ry{
            cos, 0.0,-sin, 0.0,
            0.0, 1.0, 0.0, 0.0,
            sin, 0.0, cos, 0.0,
            0.0, 0.0, 0.0, 1.0
        };
        out *= ry;
    }
    if( vRotation.z != 0.0 ) {
        auto [sin, cos] = engine::math::sin_cos( vRotation.z );
        mat4 rz{
            cos,-sin, 0.0, 0.0,
            sin, cos, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        };
        out *= rz;
    }
    return std::move(out);
}

/* mat4::translation */
mat4 mat4::translation( const vec3 &vTranslation ) {
    return mat4(
        1.0, 0.0, 0.0, vTranslation.x,
        0.0, 1.0, 0.0, vTranslation.y,
        0.0, 0.0, 1.0, vTranslation.z,
        0.0, 0.0, 0.0, 1.0
    );
}

/* mat4::perspective */
mat4 mat4::perspective( const type fov, const type aspect, const type near, const type far ) {
    auto range = near - far;
    auto halfTan = engine::math::tan( fov / 2.0 );
    return mat4(
        1.0 / (halfTan * aspect), 0.0, 0.0, 0.0,
        0.0, 1.0 / halfTan, 0.0, 0.0,
        0.0, 0.0, (-near - far) / range, 2.0 * far * near / range,
        0.0, 0.0, 1.0, 0.0
    );
}

} /* namespace math */
} /* namespace engine */