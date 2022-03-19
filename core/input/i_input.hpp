#pragma once
#include "raw_input_keys.hpp"
#include <core/math/vec2.hpp>

using namespace engine::core::math;

namespace engine::core::input
{

/* i_input interface */
class i_input
{
public:
    typedef raw_input_keys  key;
public:
    virtual         ~i_input() {}

    virtual void    attach_input() = 0;

    virtual void    detach_input() = 0;

    virtual bool    is_attached() = 0;

    //virtual bool    on_click( key ) { return false; } // Генерируется, когда пользователь щёлкнул по форме.

    //virtual bool    on_dbl_click( key ) { return false; } // Генерируется, когда пользователь дважды щёлкнул по окну.

    virtual bool    on_key_down( key ) { return false; } // Генерируется, когда нажата клавиша на клавиатуре.;

    //virtual bool    on_key_press( key ) { return false; } // Генерируется, когда нажата и отпущена клавиша на клавиатуре.

    virtual bool    on_key_up( key ) { return false; } // Генерируется, когда отпущена клавиша на клавиатуре.

    virtual bool    on_mouse_down( key ) { return false; } // Генерируется, когда нажата кнопка мыши.

    virtual bool    on_mouse_move( const point2d& ) { return false; } // Генерируется, когда двигается мышка.

    virtual bool    on_mouse_up( key ) { return false; } // Генерируется, когда отпускается кнопка мыши.

    virtual bool    on_mouse_whell( float ) { return false; } // OnMouseWheel Генерируется колёсиком мыши.

}; /* class i_input */

} /* namespace engine::core::input */
