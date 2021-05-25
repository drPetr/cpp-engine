#pragma once
#include "camera.h"
#include <core/raw_input.h>
using namespace engine::math;
using namespace engine::input;
namespace engine {

class controlled_camera : 
        public camera, 
        public input::raw_input {
public:
                    controlled_camera( const vec3 &pos, const vec3 &dir, const vec3 &up ) :
                            camera( pos, dir, up ) {}

    virtual bool    on_mouse_move( const vec2 &move ) override;
    mat4            operator()();
private:
};

}