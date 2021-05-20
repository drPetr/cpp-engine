#include <iostream>
#include <windows.h>
#include <wingdi.h>
#include <cmath>

#include <core/assert.h>
#include <core/timer.h>
#include <core/math.h>
#include <core/file_system.h>
#include <core/types.h>
#include <renderer/opengl/gl.h>
#include <core/raw_input.h>
#include <engine/camera.h>
#include <engine/object3d_location.h>
using namespace engine::math;
using namespace engine::input;

namespace engine {


bool appIsRun = true;

static LRESULT CALLBACK main_wnd_proc( HWND hwnd, UINT msg, WPARAM w, LPARAM l ) {
    switch( msg ) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_INPUT:
            raw_input::process_input( l );
            return true;
        case WM_DESTROY:
            appIsRun = false;
            return 0;
            
    }
    return DefWindowProc( hwnd, msg, w, l );
}


class window {
public:
                    window();
                    ~window();

    void            create();
    void            destroy();
    void            show();
    void            hide();
    void            align_center();

    void            set_title( const string &title );
    void            set_location( const point2d &point );
    void            set_size( const size2d &size );
    const point2d   &get_location();
    const size2d    &get_size();

    whandle_t       get_handle() const;

    void            process_messages();

                    /* получить соотношение сторон */
    float           get_aspect();

protected:
    string          title = {"window"};
    bool            isCreate = {false};
    whandle_t       handle = {nullptr};
    rect2d          rect = {{0, 0}, {854, 480}};
};



/* window::window */
window::window() {}

/* window::~window */
window::~window() {
    destroy();
}

/* window::create */
void window::create() {
    assert(!isCreate);
    static bool wcexIsInit = false;
    static WNDCLASSEX wcex;
    if( !wcexIsInit ) {
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_OWNDC;
        wcex.lpfnWndProc = static_cast<WNDPROC>(main_wnd_proc);
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = (HINSTANCE)GetModuleHandleA( nullptr );
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wcex.lpszMenuName =  NULL;
        wcex.lpszClassName = "__UserWindowClassName";
        wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        if( !RegisterClassEx(&wcex) ) {
            assert(0);
            exit(1);
        }
        wcexIsInit = true;
    }

    handle = CreateWindowEx(
        0,
        "__UserWindowClassName",
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        rect.position.x, rect.position.y, rect.size.width, rect.size.height,
        NULL,   /* (HWND) parent window */
        NULL,   /* (HMENU) window menu */
        wcex.hInstance,
        NULL
    );
    assert( handle );
    raw_input::initialize( handle );
    isCreate = true;
}

/* window::destroy */
void window::destroy() {
    raw_input::initialize( 0 );
    DestroyWindow( handle );
    handle = nullptr;
    isCreate = false;
}

/* window::show */
void window::show() {
    if( isCreate ) {
        ShowWindow( handle, SW_SHOWNORMAL );
    }
}

/* window::hide */
void window::hide() {
    if( isCreate ) {
        ShowWindow( handle, SW_SHOWMINIMIZED );
    }
}

/* window::align_center */
void window::align_center() {
    if( RECT rcScreen; GetClientRect( GetDesktopWindow(), &rcScreen ) ) {
        size2d size = get_size();
        point2d location { (rcScreen.right - size.width) / 2, 
                (rcScreen.bottom - size.height) / 2 };
        set_location( location );
    }
}

/* window::set_title */
void window::set_title( const string &title ) {
    this->title = title;
    if( isCreate ) {
        SetWindowTextA( handle, title.c_str() );
    }
}

/* window::set_location */
void window::set_location( const point2d &point ) {
    rect.position = point;
    if( isCreate ) {
        MoveWindow( handle, point.x, point.y, 
                rect.size.width, rect.size.height, FALSE );
    }
}

/* window::set_size */
void window::set_size( const size2d &size ) {
    rect.size = size;
    if( isCreate ) {
        MoveWindow( handle, rect.position.x, rect.position.y, 
                size.width, size.height, FALSE );
    }
}

/* window::get_location */
const point2d &window::get_location() {
    if( RECT rc; isCreate && GetClientRect( handle, &rc ) ) {
        rect.position.x = rc.left;
        rect.position.y = rc.top;
    }
    return rect.position;
}

/* window::get_size */
const size2d &window::get_size() {
    if( RECT rc; isCreate && GetClientRect( handle, &rc ) ) {
        rect.size.width = rc.right - rc.left;
        rect.size.height = rc.bottom - rc.top;
    }
    return rect.size;
}

/* window::get_handle */
whandle_t window::get_handle() const {
    return handle;
}

/* window::get_aspect */
float window::get_aspect() {
    auto size = get_size();
    return static_cast<float>(size.width) / static_cast<float>(size.height);
}

/* window::process_messages 
* process windows messages 
*/
void window::process_messages() {
    bool peek;
    MSG msg;
    do {
        if( (peek = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) ) {
            if( msg.message == WM_QUIT ) {
            } else {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
    } while( peek );
}









/*
::SetCursor( hcurSave );
::SetFocus( win32.hWnd );
*/

class irender {
public:
    virtual             ~irender() {}
    virtual void        display_frame() = 0;
private:
};




class opengl_render : public irender {
public:
                        opengl_render( const whandle_t handle );
    virtual             ~opengl_render();

    virtual void        display_frame() override;
private:
    vector<GLuint>      shaders;
    HDC                 hdc;
    HGLRC               hrc;
    whandle_t           hWnd;
};

/* opengl_render::opengl_render */
opengl_render::opengl_render( const whandle_t handle ) {
    assert( handle );
    /* get the device context (DC) */
    hWnd = handle;
    hdc = GetDC( hWnd );
    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int fmt = ChoosePixelFormat( hdc, &pfd );
    SetPixelFormat( hdc, fmt, &pfd );
 
    /* create and enable the render context (RC) */
    hrc = wglCreateContext( hdc );
    wglMakeCurrent( hdc, hrc );
}

/* opengl_render::~opengl_render */
opengl_render::~opengl_render() {
    ::wglMakeCurrent( NULL, NULL );
    ::wglDeleteContext( hrc );
    ::ReleaseDC( hWnd, hdc );
}

/* opengl_render::display_frame */
void opengl_render::display_frame() {
    ::SwapBuffers( hdc );
}


// display buffer
//





typedef unsigned short      vindex_t; /* vertex index type */
#define V_INDEX_TYPE        GL_UNSIGNED_SHORT

typedef unsigned char       byte_t;






GLuint compile_shader( const string &name, GLenum type ) {
    auto shader = glCreateShader( type );
    if( !shader ) {
        std::cerr << "compile_shader() error: glCreateShader()" << std::endl;
        return 0;
    }
    auto [success, contents] = core::fs.read_contents( name );
    if( success ) {
        auto *shaderText = contents.c_str();
        auto shaderTextLength = static_cast<GLint>( contents.length() );
        glShaderSource( shader, 1, &shaderText, &shaderTextLength );
        /* compile shader and print errors */
        glCompileShader( shader );
        GLint status;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
        if( !status ) {
            GLchar log[ 1024 ];
            glGetShaderInfoLog( shader, sizeof(log), NULL, log );
            std::cerr << "compile_shader() error: glCompileShader()" << 
                    log << std::endl;
            glDeleteShader( shader );
        }
    } else {
        std::cerr << "compile_shader() error: file '" << name << 
                "' not found" << std::endl;
    }
    return shader;
}

GLuint make_shader_program( const string &vshName, const string &pshName ) {
    auto program = glCreateProgram();
    if( !program ) {
        std::cerr << "make_shader_program() error: glCreateProgram()" << std::endl;
        return 0;
    }
    auto vsh = compile_shader( vshName, GL_VERTEX_SHADER );
    if( !vsh ) {
        return 0;
    }
    auto psh = compile_shader( pshName, GL_FRAGMENT_SHADER );
    if( !psh ) {
        return 0;
    }
    glAttachShader( program, vsh );
    glAttachShader( program, psh );
    glLinkProgram( program );
    /* check shader program link status */
    GLint status;
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    if( status == 0 ) {
        GLchar log[ 1024 ];
        glGetProgramInfoLog( program, sizeof(log), NULL, log );
        std::cerr << "make_shader_program() error: glLinkProgram()" << 
                    log << std::endl;
        glDetachShader( program, vsh );
        glDeleteShader( vsh );
        glDetachShader( program, psh );
        glDeleteShader( psh );
        glDeleteProgram( program );
        return 0;
    }
    glDetachShader( program, vsh );
    glDeleteShader( vsh );
    glDetachShader( program, psh );
    glDeleteShader( psh );
    return program;
}


/*


    Fvector                                 vCameraPosition;
    Fvector                                 vCameraDirection;
    Fvector                                 vCameraTop;
    Fvector                                 vCameraRight;

    Fmatrix                 mView;
    Fmatrix                 mProject;
    Fmatrix                 mFullTransform;

    // Copies of corresponding members. Used for synchronization.
    Fvector                         vCameraPosition_saved;

    Fmatrix                     mView_saved;
    Fmatrix                     mProject_saved;
    Fmatrix                     mFullTransform_saved;
*/






/* local_object 
* Локальные координаты это координаты вашего объекта измеряемые относительно
* точки отсчета расположенной там, где начинается сам объект
* LOCAL SPACE mul (MODEL MATRIX) -> WORLD SPACE
*/
class local_object {
public:
};

/* world_object
* На следующем шаге локальные координаты преобразуются в координаты мирового
* пространства, которые по смыслу являются координатами более крупного мира. 
* Эти координаты измеряются относительно глобальной точки отсчёта, единой для 
* всех других объектов расположенных в мировом пространстве.
* WORLD SPACE mul (VIEV MATRIX) -> VIEW SPACE
*/
class world_object {
public:

};



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

/* controlled_camera::on_mouse_move */
bool controlled_camera::on_mouse_move( const vec2 &move ) {
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

/* controlled_camera::operator() */
mat4 controlled_camera::operator()() {
    if( controlled_camera::is_key_pressed(VKRAW_W) ) {
        move( get_direction() * 0.1 );
    }
    if( controlled_camera::is_key_pressed(VKRAW_S) ) {
        move( -get_direction() * 0.1 );
    }
    if( controlled_camera::is_key_pressed(VKRAW_A) ) {
        move( -get_right() * 0.1 );
    }
    if( controlled_camera::is_key_pressed(VKRAW_D) ) {
        move( get_right() * 0.1 );
    }
    if( controlled_camera::is_key_pressed(VKRAW_SHIFT) ) {
        move( -get_up() * 0.1 );
    }
    if( controlled_camera::is_key_pressed(VKRAW_SPACE) ) {
        move( get_up() * 0.1 );
    }
    return camera::operator()();
}





} /* namespace engine */

using namespace engine;
 
#define __unused(v)   static_cast<void>(v)

int WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow ) {
    __unused(hInst);
    __unused(hPrevInst);
    __unused(lpCmdLine);
    __unused(nCmdShow);

    window w;
    w.set_title( "Main window" );
    w.set_size( {1280, 720} );
    w.align_center();
    w.create();
    w.show();

    irender *r = new opengl_render( w.get_handle() );
    
/*    GLint len = static_cast<contents.length()>;
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
*/
    
    auto shProgram = make_shader_program( "shader.vsh", "shader.psh" );
    glValidateProgram( shProgram );
    GLint status;
    glGetProgramiv( shProgram, GL_VALIDATE_STATUS, &status );
    if( status == 0 ) {
        GLchar log[ 1024 ];
        glGetProgramInfoLog( shProgram, sizeof(log), NULL, log );
        std::cerr << "glValidateProgram() error:" << log << std::endl;
    }


    auto gWorldLocation = glGetUniformLocation( shProgram, "gWorld" );
    assert( gWorldLocation != static_cast<int>(0xFFFFFFFF) );


    GLuint axesVbo[3];
    vec3 xAxis[]{ {0.0, 0.0, 0.0}, {5.0, 0.0, 0.0} };
    vec3 yAxis[]{ {0.0, 0.0, 0.0}, {0.0, 5.0, 0.0} };
    vec3 zAxis[]{ {0.0, 0.0, 0.0}, {0.0, 0.0, 5.0} };
    


    

    vec3 v[] {
        /*{0.5f,  0.5f, 0.0f},  // Верхний правый угол
        {0.5f, -0.5f, 0.0f},  // Нижний правый угол
        {-0.5f, -0.5f, 0.0f},  // Нижний левый угол
        {-0.5f,  0.5f, 0.0f}   // Верхний левый угол
        */
        {-1.0, -1.0, -1.0},
        {-1.0, -1.0, 1.0},
        {1.0, -1.0, -1.0},
        {1.0, -1.0, 1.0},
        {-1.0, 1.0, -1.0},
        {-1.0, 1.0, 1.0},
        {1.0, 1.0, -1.0},
        {1.0, 1.0, 1.0} 
        /*{-1.0, -1.0, 0.0},
        {0.0, -1.0, 1.0},
        {1.0, -1.0, 0.0},
        {0.0, 1.0, 0.0}*/
    };

    unsigned int indices[] = {
        0,1,2,1,2,3,2,3,7,7,2,6,6,7,5,6,5,4,5,4,0,5,0,1,0,4,6,0,6,2,1,5,7,1,7,3
        //0, 1, 3,   // Первый треугольник
        //1, 2, 3    // Второй треугольник
        //0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2
    };

    GLuint vbo; /* vertex buffer object */
    GLuint ibo; /* index buffer object (Element Buffer Object) */
    GLuint vao;


    glGenVertexArrays( 1, &vao );
    {
        /* init mesh (vao)*/
        glBindVertexArray( vao );

        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof(v), v[0].get_ptr(), GL_STATIC_DRAW );

        glGenBuffers( 1, &ibo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), NULL );
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);



    glClearColor( 0.0f, 0.3f, 0.3f, 0.0f );


    mat4 world;
    world.identity();
    //glUniformMatrix4fv( gWorldLocation, 1, GL_TRUE, world.get_ptr() );

    core::timer timer;


    static float fscale = 0.0;
    static float fpos = 0.0;
    static float frot = 0.0;
    object3d_location loc;
    object3d_location loc2;
    controlled_camera cam( vec3(0,0,0), vec3(0,1,0), vec3(0,0,1) );
    cam.attach_input();

    //cam.set_position( vec3( 0, -5, 0 ) );

    static float pi = 3.14159265358979;
    bool pause = false;
    bool change = true;
    bool prev = false;
    float stepSize = 0.1;


    while( appIsRun ) {
        if( raw_input::is_key_pressed(VKRAW_ESCAPE) ) {
            appIsRun = false;
        }

        if( prev != raw_input::is_key_pressed(VKRAW_Q) ) {
            std::cout <<"change" << std::endl;
            if( change ) {
                pause = !pause;
                change = false;
            } else {
                change = true;
            }
            if( pause ) {
                //w.set_title( "pause" );
            } else {
                //w.set_title( "run" );
            }
        }
        prev = raw_input::is_key_pressed(VKRAW_Q);


        if( pause ) {
            timer.time_sec();
            w.process_messages();
            Sleep(1);
            r->display_frame();

            continue;
        }


        
        

        auto msec = timer.time_sec();
        w.process_messages();

        glViewport( 0, 0, w.get_size().width, w.get_size().height );


        fscale += 3.14159265358979 * msec * 0.25;
        fpos += 3.14159265358979 * msec * 0.3333333333333333333333;
        frot += 3.14159265358979 * msec *0.5;

        auto [sin, cos] = engine::math::sin_cos( fpos );
        loc.set_scale( vec3(0.5,  0.5,  0.5) );
        loc.set_position( vec3(0.0, 10.0, 0.0) );

        loc2.set_scale( vec3(1.0,  1.0,  1.0) );
        loc2.set_position( vec3(10.0, 0.0, 0.0) );
        


        //cam.move( vec3(cos * 0.1, sin * 0.1, 0) );
        auto perspective = mat4::perspective( pi / 180.0 * 60.0, w.get_aspect(), 0.1, 1000 );

        

        //std::cout << "[" << ccc.z.x << ", " << ccc.z.y << ", " << ccc.z.z << "]" << std::endl;

        glClear( GL_COLOR_BUFFER_BIT );



        auto toShader = perspective * cam() * loc();
        /* send world matrix to shader */
        glUniformMatrix4fv( gWorldLocation, 1, GL_TRUE, toShader.get_ptr() );

        glUseProgram( shProgram );
        glBindVertexArray( vao );
        glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );

        toShader = perspective * cam() * loc2();
        glUniformMatrix4fv( gWorldLocation, 1, GL_TRUE, toShader.get_ptr() );
        glUseProgram( shProgram );
        glBindVertexArray( vao );
        glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );




        auto skip = static_cast<int>(1000.0 / 60.0 - timer.get_elapsed_msec());
        Sleep( skip > 0 ? skip : 0 );

        r->display_frame();
    }    
    


    delete r;


    return 0;
}
