#include "raw_input.h"
#include <core/assert.h>
#include <hidusage.h>
#include <iostream>
namespace engine {
namespace input {

vec2 raw_input::mouseDelta;
bool raw_input::keysPressed[256];
bool raw_input::isInit {false};
POINT lastPos{0,0};
bool locked = false;

/* raw_input::initialize */
void raw_input::initialize( whandle_t handle ) {
    assert( (handle && !isInit) || (!handle && isInit)  );
    static RAWINPUTDEVICE devieMouse;
    static RAWINPUTDEVICE devieKeyboard;
    if( handle && !isInit ) {
        mouseDelta.zero();
        for( int i = 0; i < static_cast<int>(sizeof(keysPressed)); i++ ) {
            keysPressed[i] = false;
        }
        /* initialize mouse input */
        {
            devieMouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
            devieMouse.usUsage = HID_USAGE_GENERIC_MOUSE;
            devieMouse.dwFlags = 0;
            devieMouse.hwndTarget = handle;
            auto success = RegisterRawInputDevices( &devieMouse, 1, sizeof(devieMouse) );
            assert( success );
        }
        /* initialize keybord input */
        {
            devieKeyboard.usUsagePage = HID_USAGE_PAGE_GENERIC;
            devieKeyboard.usUsage = HID_USAGE_GENERIC_KEYBOARD;
            devieKeyboard.dwFlags = /*RIDEV_NOLEGACY | RIDEV_NOHOTKEYS*/ 0;
            devieKeyboard.hwndTarget = handle;
            auto success = RegisterRawInputDevices( &devieKeyboard, 1, sizeof(devieKeyboard) );
            assert( success );
        }
        isInit = true;
    } else if( !handle && isInit ) {
        /* delete mouse input */
        {
            devieMouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
            devieMouse.usUsage = HID_USAGE_GENERIC_MOUSE;
            devieMouse.dwFlags = RIDEV_REMOVE;
            devieMouse.hwndTarget = handle;
            auto success = RegisterRawInputDevices( &devieMouse, 1, sizeof(devieMouse) );
            assert( success );
        }
        /* delete keybord input */
        {
            devieKeyboard.usUsagePage = HID_USAGE_PAGE_GENERIC;
            devieKeyboard.usUsage = HID_USAGE_GENERIC_KEYBOARD;
            devieKeyboard.dwFlags = RIDEV_REMOVE;
            devieKeyboard.hwndTarget = handle;
            auto success = RegisterRawInputDevices( &devieKeyboard, 1, sizeof(devieKeyboard) );
            assert( success );
        }
        isInit = false;
    }
}

/* raw_input::is_key_pressed */
bool raw_input::is_key_pressed( key dik ) {
    return keysPressed[dik];
}

/* raw_input::get_mouse_position */
const vec2 &raw_input::get_mouse_position() {
    return mouseDelta;
}

/* raw_input::lock_mouse */
void raw_input::lock_mouse() {
    ::ShowCursor(FALSE);
    GetCursorPos( &lastPos );
    locked = true;
    std::cout << "lock_mouse()" << std::endl;
}

/* raw_input::lock_mouse */
void raw_input::unlock_mouse() {
    ::ShowCursor(TRUE);
    SetCursorPos( lastPos.x, lastPos.y );
    locked = false;
    std::cout << "unlock_mouse()" << std::endl;
}

/* raw_input::process_input */
void raw_input::process_input( LPARAM hRawInput ) {
    assert( isInit );
    unsigned char buffer[1024 * 8]; /* should have */
    RAWINPUT *rawInput = reinterpret_cast<RAWINPUT*>(buffer);
    UINT rawInputSize = static_cast<UINT>(sizeof(buffer));
    auto bytes = GetRawInputData( (HRAWINPUT)hRawInput, RID_INPUT, rawInput, &rawInputSize, sizeof(RAWINPUTHEADER));
    assert( bytes != 0x80000000 );

    switch( RAWINPUTHEADER &header = rawInput->header; header.dwType ) {
        case RIM_TYPEMOUSE: {
            RAWMOUSE &mouseData = rawInput->data.mouse;
            if( mouseData.usButtonFlags & RI_MOUSE_WHEEL ) {
                /* process mouse whell */
                short whellTicks = mouseData.usButtonData & 0xffff;
                float wheelDelta = static_cast<float>(whellTicks);

            }

            if( mouseData.lLastX != 0 || mouseData.lLastY != 0 ) {
                /* process mouse move (relative coordinates) */
                mouseDelta = vec2( mouseData.lLastX , mouseData.lLastY  );
                if( locked ) {
                    SetCursorPos( lastPos.x, lastPos.y );
                }
            }

            /* check mouse button states */
            struct keys_struct {
                key         vkey;       /* virtual key */
                USHORT      downFlag;
                USHORT      upFlag;
            };
            static keys_struct keys[] {
                {VK_LBUTTON, RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP},
                {VK_RBUTTON, RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP},
                {VK_MBUTTON, RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP},
                {VK_XBUTTON1, RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP},
                {VK_XBUTTON2, RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP},
            };
            for( auto k : keys ) {
                if( mouseData.usButtonFlags & k.downFlag ) {
                    /* mouse key down process */
                    keysPressed[k.vkey] = true;
                    
                }
                if( mouseData.usButtonFlags & k.upFlag ) {
                    /* mouse key up process */
                    keysPressed[k.vkey] = false;

                }
            }
        }
        break;
        case RIM_TYPEKEYBOARD: {
            RAWKEYBOARD &keyboardData = rawInput->data.keyboard;
            USHORT &vkey = keyboardData.VKey;
            if( vkey >= 0xff ) {
                return;
            }

            if( keyboardData.Flags == RI_KEY_MAKE  ) {
                if( !keysPressed[vkey] ) {
                    keysPressed[vkey] = true;
                    std::cout << "keyboard key down: [" << int(vkey) << "]" << std::endl;
                    if( vkey == 0x51 ) {
                        lock_mouse();
                    }
                }
            } else if (keyboardData.Flags & RI_KEY_BREAK) {
                if( keysPressed[vkey] ) {
                    if( vkey == 0x51 ) {
                        unlock_mouse();
                    }
                    keysPressed[vkey] = false;
                    std::cout << "keyboard key up: [" << int(vkey) << "]" << std::endl;
                }
            }

            /*if( keysPressed[VK_F4] && keysPressed[VK_MENU] ) {
                exit(0);
            }*/
        }
        break;
    }
}

} /* namespace core */
} /* namespace engine */