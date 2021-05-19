#pragma once
#include <core/types.h>
#include <core/math.h>
namespace engine {
namespace core {

using namespace engine::math;

enum raw_input_keys {
VKRAW_LBUTTON = VK_LBUTTON,
VKRAW_RBUTTON = VK_RBUTTON,
VKRAW_CANCEL = VK_CANCEL,
VKRAW_MBUTTON = VK_MBUTTON,
VKRAW_XBUTTON1 = VK_XBUTTON1,
VKRAW_XBUTTON2 = VK_XBUTTON2,
VKRAW_BACK = VK_BACK,
VKRAW_TAB = VK_TAB,
VKRAW_CLEAR = VK_CLEAR,
VKRAW_RETURN = VK_RETURN,
VKRAW_SHIFT = VK_SHIFT,
VKRAW_CONTROL = VK_CONTROL,
VKRAW_MENU = VK_MENU,
VKRAW_PAUSE = VK_PAUSE,
VKRAW_CAPITAL = VK_CAPITAL,
VKRAW_KANA = VK_KANA,
VKRAW_HANGUL = VK_HANGUL,
VKRAW_JUNJA = VK_JUNJA,
VKRAW_FINAL = VK_FINAL,
VKRAW_HANJA = VK_HANJA,
VKRAW_KANJI = VK_KANJI,
VKRAW_ESCAPE = VK_ESCAPE,
VKRAW_CONVERT = VK_CONVERT,
VKRAW_NONCONVERT = VK_NONCONVERT,
VKRAW_ACCEPT = VK_ACCEPT,
VKRAW_MODECHANGE = VK_MODECHANGE,
VKRAW_SPACE = VK_SPACE,
VKRAW_PRIOR = VK_PRIOR,
VKRAW_NEXT = VK_NEXT,
VKRAW_END = VK_END,
VKRAW_HOME = VK_HOME,
VKRAW_LEFT = VK_LEFT,
VKRAW_UP = VK_UP,
VKRAW_RIGHT = VK_RIGHT,
VKRAW_DOWN = VK_DOWN,
VKRAW_SELECT = VK_SELECT,
VKRAW_PRINT = VK_PRINT,
VKRAW_EXECUTE = VK_EXECUTE,
VKRAW_SNAPSHOT = VK_SNAPSHOT,
VKRAW_INSERT = VK_INSERT,
VKRAW_DELETE = VK_DELETE,
VKRAW_HELP = VK_HELP,
VKRAW_0 = 0x30,
VKRAW_1,
VKRAW_2,
VKRAW_3,
VKRAW_4,
VKRAW_5,
VKRAW_6,
VKRAW_7,
VKRAW_8,
VKRAW_9,
VKRAW_A = 0x41,
VKRAW_B,
VKRAW_C,
VKRAW_D,
VKRAW_E,
VKRAW_F,
VKRAW_G,
VKRAW_H,
VKRAW_I,
VKRAW_J,
VKRAW_K,
VKRAW_L,
VKRAW_M,
VKRAW_N,
VKRAW_O,
VKRAW_P,
VKRAW_Q,
VKRAW_R,
VKRAW_S,
VKRAW_T,
VKRAW_U,
VKRAW_V,
VKRAW_W,
VKRAW_X,
VKRAW_Y,
VKRAW_Z,
VKRAW_LWIN = VK_LWIN,
VKRAW_RWIN = VK_RWIN,
VKRAW_APPS = VK_APPS,
VKRAW_SLEEP = VK_SLEEP,
VKRAW_NUMPAD0 = VK_NUMPAD0,
VKRAW_NUMPAD1 = VK_NUMPAD1,
VKRAW_NUMPAD2 = VK_NUMPAD2,
VKRAW_NUMPAD3 = VK_NUMPAD3,
VKRAW_NUMPAD4 = VK_NUMPAD4,
VKRAW_NUMPAD5 = VK_NUMPAD5,
VKRAW_NUMPAD6 = VK_NUMPAD6,
VKRAW_NUMPAD7 = VK_NUMPAD7,
VKRAW_NUMPAD8 = VK_NUMPAD8,
VKRAW_NUMPAD9 = VK_NUMPAD9,
VKRAW_MULTIPLY = VK_MULTIPLY,
VKRAW_ADD = VK_ADD,
VKRAW_SEPARATOR = VK_SEPARATOR,
VKRAW_SUBTRACT = VK_SUBTRACT,
VKRAW_DECIMAL = VK_DECIMAL,
VKRAW_DIVIDE = VK_DIVIDE,
VKRAW_F1 = VK_F1,
VKRAW_F2 = VK_F2,
VKRAW_F3 = VK_F3,
VKRAW_F4 = VK_F4,
VKRAW_F5 = VK_F5,
VKRAW_F6 = VK_F6,
VKRAW_F7 = VK_F7,
VKRAW_F8 = VK_F8,
VKRAW_F9 = VK_F9,
VKRAW_F10 = VK_F10,
VKRAW_F11 = VK_F11,
VKRAW_F12 = VK_F12,
VKRAW_F13 = VK_F13,
VKRAW_F14 = VK_F14,
VKRAW_F15 = VK_F15,
VKRAW_F16 = VK_F16,
VKRAW_F17 = VK_F17,
VKRAW_F18 = VK_F18,
VKRAW_F19 = VK_F19,
VKRAW_F20 = VK_F20,
VKRAW_F21 = VK_F21,
VKRAW_F22 = VK_F22,
VKRAW_F23 = VK_F23,
VKRAW_F24 = VK_F24,
VKRAW_NUMLOCK = VK_NUMLOCK,
VKRAW_SCROLL = VK_SCROLL,
VKRAW_LSHIFT = VK_LSHIFT,
VKRAW_RSHIFT = VK_RSHIFT,
VKRAW_LCONTROL = VK_LCONTROL,
VKRAW_RCONTROL = VK_RCONTROL,
VKRAW_LMENU = VK_LMENU,
VKRAW_RMENU = VK_RMENU,
vkraw_lastindex
};

/* input interface */
class iinput {
public:
    typedef raw_input_keys  key;
public:
    bool            is_key_pressed( key dik );

    virtual void    on_click( key dik ); // Генерируется, когда пользователь щёлкнул по форме.

    virtual void    on_dbl_click( key dik ); // Генерируется, когда пользователь дважды щёлкнул по окну.

    virtual void    on_key_down( key dik ); // Генерируется, когда нажата клавиша на клавиатуре.;

    virtual void    on_key_press( key dik ); // Генерируется, когда нажата и отпущена клавиша на клавиатуре.

    virtual void    on_key_up( key dik ); // Генерируется, когда отпущена клавиша на клавиатуре.

    virtual void    on_mouse_down( key dik ); // Генерируется, когда нажата кнопка мыши.

    virtual void    on_mouse_move( const vec2 &move ); // Генерируется, когда двигается мышка.

    virtual void    on_mouse_up( key dik ); // Генерируется, когда отпускается кнопка мыши.

    virtual void    on_mouse_whell( float whell ); // OnMouseWheel Генерируется колёсиком мыши.

};

/* singleton object */
class raw_input {
    typedef unsigned char  key;
public:
    static void         initialize( whandle_t handle );

    static bool         is_key_pressed( key dik );
    static const vec2   &get_mouse_position();

    static void         lock_mouse();
    static void         unlock_mouse();
    static void         process_input( LPARAM hRawInput );
private:
    static vec2         mouseDelta;
    static bool         keysPressed[256];
    static bool         isInit;

    //RECT cachedWindowRect;
};

} /* namespace core */
} /* namespace engine */