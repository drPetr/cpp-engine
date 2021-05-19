#include "raw_input.h"
#include <core/assert.h>
#include <hidusage.h>
#include <algorithm>
namespace engine {
namespace input {

bool raw_input::keysPressed[256];
bool raw_input::isInit {false};
vector<iinput*>  focusStack;

/* raw_input::attach_input */
void raw_input::attach_input() {
    assert( !is_attached() );
    focusStack.push_back( dynamic_cast<iinput*>(this) );
    attached = true;
}

/* raw_input::detach_input */
void raw_input::detach_input() {
    assert( is_attached() );
    auto inp( dynamic_cast<iinput*>(this) );
    if( focusStack.back() == inp ) {
        focusStack.pop_back();
    } else {
        const auto endit( remove(focusStack.begin(), focusStack.end(), inp) );
        focusStack.erase( endit, focusStack.end() );
    }
    attached = false;
}

/* raw_input::initialize */
void raw_input::initialize( whandle_t handle ) {
    assert( (handle && !isInit) || (!handle && isInit)  );
    static RAWINPUTDEVICE devieMouse;
    static RAWINPUTDEVICE devieKeyboard;
    if( handle && !isInit ) {
        for( int i = 0; i < static_cast<int>(sizeof(keysPressed)); i++ ) {
            keysPressed[i] = false;
        }
        /* initialize mouse input */
        {
            devieMouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
            devieMouse.usUsage = HID_USAGE_GENERIC_MOUSE;
            devieMouse.dwFlags = 0;
            devieMouse.hwndTarget = handle;
            auto success( RegisterRawInputDevices( &devieMouse, 1, sizeof(devieMouse) ) );
            assert( success );
        }
        /* initialize keybord input */
        {
            devieKeyboard.usUsagePage = HID_USAGE_PAGE_GENERIC;
            devieKeyboard.usUsage = HID_USAGE_GENERIC_KEYBOARD;
            devieKeyboard.dwFlags = /*RIDEV_NOLEGACY | RIDEV_NOHOTKEYS*/ 0;
            devieKeyboard.hwndTarget = handle;
            auto success( RegisterRawInputDevices( &devieKeyboard, 1, sizeof(devieKeyboard) ) );
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

/* raw_input::process_input */
void raw_input::process_input( LPARAM hRawInput ) {
    assert( isInit );
    unsigned char buffer[1024 * 8]; /* should have */
    RAWINPUT *rawInput( reinterpret_cast<RAWINPUT*>(buffer) );
    UINT rawInputSize( static_cast<UINT>(sizeof(buffer)) );
    auto bytes( GetRawInputData( (HRAWINPUT)hRawInput, RID_INPUT, rawInput, &rawInputSize, sizeof(RAWINPUTHEADER)) );
    assert( bytes != 0x80000000 );

    switch( RAWINPUTHEADER &header = rawInput->header; header.dwType ) {
        case RIM_TYPEMOUSE: {
            RAWMOUSE &mouseData = rawInput->data.mouse;
            if( mouseData.usButtonFlags & RI_MOUSE_WHEEL ) {
                /* process mouse whell */
                short whellTicks = mouseData.usButtonData & 0xffff;
                float wheelDelta( static_cast<float>(whellTicks) );
                for( auto it = focusStack.rbegin(); it != focusStack.rend(); it++ ) {
                    if( (*it)->on_mouse_whell( wheelDelta ) ) {
                        break;
                    }
                }
            }

            if( mouseData.lLastX != 0 || mouseData.lLastY != 0 ) {
                /* process mouse move (relative coordinates) */
                auto mouseDelta( vec2( mouseData.lLastX , mouseData.lLastY ) );
                for( auto it = focusStack.rbegin(); it != focusStack.rend(); it++ ) {
                    if( (*it)->on_mouse_move( mouseDelta ) ) {
                        break;
                    }
                }
            }

            /* check mouse button states */
            struct keys_struct {
                key         vkey;       /* virtual key */
                USHORT      downFlag;
                USHORT      upFlag;
            };
            static keys_struct keys[] {
                {VKRAW_LBUTTON, RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP},
                {VKRAW_RBUTTON, RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP},
                {VKRAW_MBUTTON, RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP},
                {VKRAW_XBUTTON1, RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP},
                {VKRAW_XBUTTON2, RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP},
            };
            for( auto k : keys ) {
                if( mouseData.usButtonFlags & k.downFlag ) {
                    /* mouse key down process */
                    keysPressed[k.vkey] = true;
                    for( auto it = focusStack.rbegin(); it != focusStack.rend(); it++ ) {
                        if( (*it)->on_mouse_down( k.vkey ) ) {
                            break;
                        }
                    }
                }
                if( mouseData.usButtonFlags & k.upFlag ) {
                    /* mouse key up process */
                    keysPressed[k.vkey] = false;
                    for( auto it = focusStack.rbegin(); it != focusStack.rend(); it++ ) {
                        if( (*it)->on_mouse_up( k.vkey ) ) {
                            break;
                        }
                    }
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
                    for( auto it = focusStack.rbegin(); it != focusStack.rend(); it++ ) {
                        if( (*it)->on_key_down( static_cast<key>(vkey) ) ) {
                            break;
                        }
                    }
                }
            } else if (keyboardData.Flags & RI_KEY_BREAK) {
                if( keysPressed[vkey] ) {
                    keysPressed[vkey] = false;
                    for( auto it = focusStack.rbegin(); it != focusStack.rend(); it++ ) {
                        if( (*it)->on_key_up( static_cast<key>(vkey) ) ) {
                            break;
                        }
                    }
                }
            }
        }
        break;
    }
}

} /* namespace core */
} /* namespace engine */