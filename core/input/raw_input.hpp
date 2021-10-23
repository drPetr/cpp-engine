#pragma once
#include "i_input.hpp"
#include <core/types.hpp>
#include <core/vector.hpp>
#include <core/math.hpp>

namespace engine::core::input
{

class raw_input : public i_input
{
public:
    virtual             ~raw_input();

    virtual void        attach_input() override;

    virtual void        detach_input() override;

    virtual bool        is_attached() override;

    static void         initialize( whandle_t handle );

    static bool         is_key_pressed( key dik );

    static void         process_input( LPARAM hRawInput );
private:
    static bool         keysPressed[256];
    static bool         isInit;
    static core::vector<i_input*> focusStack;
    bool                attached{false};
}; /* class raw_input */

/* raw_input::is_attached */
inline bool raw_input::is_attached()
{
    return attached;
}

/* raw_input::is_key_pressed */
inline bool raw_input::is_key_pressed( key dik )
{
    return keysPressed[dik];
}

} /* namespace engine::core::input */
