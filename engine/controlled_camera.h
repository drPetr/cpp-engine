#pragma once
#include "camera.h"
#include <core/input.hpp>
using namespace engine::core::math;

namespace engine {

class controlled_camera : 
        public camera, 
        public core::input::raw_input
{
public:
                    controlled_camera( const vec3 &pos, const vec3 &dir, const vec3 &up ) :
                            camera( pos, dir, up ) {}

    void            update_movement();

    virtual bool    on_mouse_move( const point2d &move ) override;
private:
};

}