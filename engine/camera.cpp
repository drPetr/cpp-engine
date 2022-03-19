#include "camera.h"
namespace engine {
using namespace core::math;

const mat4 MAT4_CAMERA_INITIAL {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};

/* camera::camera */
camera::camera( const vec3 &pos, const vec3 &dir, const vec3 &up )
{
    set_position( pos );
    set_direction( dir );
    set_up( up );
}

/* camera::rotate */
void camera::rotate( const quat &q )
{
    set_direction( q * get_direction() );
    set_up( q * get_up() );
    m_needUpdate = true;
}

/* camera::set_orientation */
void camera::set_orientation( const vec3 &dir, const vec3 &up )
{
    m_dir = dir;
    m_dir.normalize();
    m_up = up;
    m_up.normalize();
    m_right = m_dir.cross( m_up );
    m_up = m_right.cross( m_dir );
    m_needUpdate = true;
}

/* camera::set_direction */
void camera::set_direction( const vec3 &dir )
{
    m_dir = dir;
    m_dir.normalize();
    /* update right */
    m_right = m_dir.cross( m_up );
    /* update up */
    m_up = m_right.cross( m_dir );
    m_needUpdate = true;
}

/* camera::set_up */
void camera::set_up( const vec3 &up )
{
    m_up = up;
    m_up.normalize();
    /* update right */
    m_right = m_dir.cross( m_up );
    /* update up */
    m_up = m_right.cross( m_dir );
    m_needUpdate = true;
}

/* camera::get_scale */
mat4 camera::operator()()
{
    if (m_needUpdate) {
        m_out = m_mat;
        /* scaling */
        m_out.x.x *= m_scale.x;
        m_out.y.y *= m_scale.y;
        m_out.z.z *= m_scale.z;
        /* optimization (multiply camera matrix on position matrix) */
        m_out.x.w = m_right.x * m_pos.x + m_right.y * m_pos.y + m_right.z * m_pos.z;
        m_out.y.w = m_up.x * m_pos.x + m_up.y * m_pos.y + m_up.z * m_pos.z;
        m_out.z.w = m_dir.x * m_pos.x + m_dir.y * m_pos.y + m_dir.z * m_pos.z;
        m_out = m_projectionMat * m_out;
        m_needUpdate = false;
        return m_out;
    }
    return m_out;
}

} /* namespace engine */
