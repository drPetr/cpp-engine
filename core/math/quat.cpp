#include "quat.hpp"

namespace engine::core::math
{

const quat QUAT_ZERO { 0.0, 0.0, 0.0, 1.0 };

/* quat::to_mat4 */
mat4 quat::to_mat4()
{
    auto x2 = x + x;
    auto y2 = y + y;
    auto z2 = z + z;
    auto xx = x * x2;
    auto xy = x * y2;
    auto xz = x * z2;
    auto yy = y * y2;
    auto yz = y * z2;
    auto zz = z * z2;
    auto wx = w * x2;
    auto wy = w * y2;
    auto wz = w * z2;
    return mat4(
        1.0 - (yy + zz), xy - wz, xz + wy, 0.0,
        xy + wz, 1.0 - ( xx + zz ), yz - wx, 0.0,
        xz - wy, yz + wx, 1.0f - ( xx + yy ), 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

} /* namespace engine::core::math */
