#include "controlled_camera.h"

using namespace engine::core::input;

namespace engine {

using namespace core::math;

/* controlled_camera::on_mouse_move */
bool controlled_camera::on_mouse_move( const point2d &move ) {
    auto m = move / 500;
    if( m.x ) {
        quat q( get_up(), m.x );
        set_direction( q * get_direction() );
    }
    if( m.y ) {
        quat q( get_right(), m.y );
        set_direction( q * get_direction() );
        set_up( q * get_up() );

    }
    return true;
}

/* controlled_camera::update_movement */
void controlled_camera::update_movement() {
    vec3 mov{0.0, 0.0, 0.0};
    if( raw_input::is_key_pressed(VKRAW_W) ) {
        mov += get_direction();
    }
    if( raw_input::is_key_pressed(VKRAW_S) ) {
        mov -= get_direction();
    }
    if( raw_input::is_key_pressed(VKRAW_D) ) {
        mov += get_right();
    }
    if( raw_input::is_key_pressed(VKRAW_A) ) {
        mov -= get_right();
    }
    if( raw_input::is_key_pressed(VKRAW_SPACE) ) {
        mov += get_up();
    }
    if( raw_input::is_key_pressed(VKRAW_SHIFT) ) {
        mov -= get_up();
    }
    move( mov * 0.25 );
}

} /* namespace engine */