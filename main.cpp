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


class transform {
public:
    typedef float   type;
public:
                transform() : 
                        scale{1.0, 1.0, 1.0}, 
                        rotation{0.0, 0.0, 0.0},
                        position{0.0, 0.0, 0.0} {}
                transform( const vec3 &scale, const vec3 &rotation, const vec3 &position ) :
                        scale{scale}, rotation{rotation}, position{position} {}


    void        set_scale( const vec3 &scale );
    void        set_scale( const type xScale, const type yScale, const type zScale );
    void        set_rotation( const vec3 &rotation );
    void        set_rotation( const type xRotate, const type yRotate, const type zRotate );
    void        set_position( const vec3 &position );    
    void        set_position( const type x, const type y, const type z );

    vec3        &get_scale();
    const vec3  &get_scale() const;
    vec3        &get_rotation();
    const vec3  &get_rotation() const;
    vec3        &get_position();
    const vec3  &get_position() const;

    mat4        &operator()();


private:
    vec3 scale;
    vec3 rotation;
    vec3 position;
    mat4 out;
};

/* transform::set_scale */
void transform::set_scale( const vec3 &scale ) {
    this->scale = scale;
}

/* transform::set_scale */
void transform::set_scale( const type xScale, const type yScale, const type zScale ) {
    scale = vec3( xScale, yScale, zScale );
}

/* transform::set_rotation */
void transform::set_rotation( const vec3 &rotation ) {
    this->rotation = rotation;
}

/* transform::set_rotation */
void transform::set_rotation( const type xRotate, const type yRotate, const type zRotate ) {
    rotation = vec3( xRotate, yRotate, zRotate );
}

/* transform::set_position */
void transform::set_position( const vec3 &position ) {
    this->position = position;
}

/* transform::set_position */
void transform::set_position( const type x, const type y, const type z ) {
    position = vec3( x, y, z );
}

/* transform::get_scale */
vec3 &transform::get_scale() {
    return scale;
}

/* transform::get_scale */
const vec3  &transform::get_scale() const {
    return scale;
}

/* transform::get_rotation */
vec3 &transform::get_rotation() {
    return rotation;
}

/* transform::get_rotation */
const vec3 &transform::get_rotation() const {
    return rotation;
}

/* transform::get_position */
vec3 &transform::get_position() {
    return position;
}

/* transform::get_position */
const vec3 &transform::get_position() const {
    return position;
}

/* transform::operator() */
mat4 &transform::operator()() {
    out = mat4::translation(position) * 
        mat4::rotation(rotation) * 
        mat4::scale(scale);
    return out;
}












class camera {
public:
                    camera() :
                            position{0.0, 0.0, 0.0},
                            direction{0.0, 0.0, 1.0},
                            up{0.0, 1.0, 0.0},
                            right{1.0, 0.0, 0.0} {}

                    camera( const vec3 &pos, const vec3 &dir, const vec3 &up );

    void            move( const vec3 &delta );
    void            rotate( const vec3 &delta );

    void            set_position( const vec3 &position );
    void            set_direction( const vec3 &direction );
    void            set_up( const vec3 &up );

    const vec3      &get_position() const;
    const vec3      &get_direction() const;
    const vec3      &get_up();
    const vec3      &get_right();

    mat4            &operator()();

private:
    vec3        position;   /* camera position */
    vec3        direction;  /* camera direction */
    vec3        up;         /* camera up vector */
    vec3        right;      /* camera right vector */
    mat4        out;        /* output camera matrix */
};

/* camera::camera */
camera::camera( const vec3 &pos, const vec3 &dir, const vec3 &up ) {
    this->position = pos;
    this->direction = dir;
    this->up = up;
    this->direction.normalize();
    this->up.normalize();
    this->right = this->up.cross( this->direction );
    this->up = this->direction.cross( this->right );
}

/* camera::move */
void camera::move( const vec3 &delta ) {
    this->position += delta;
}

/* camera::rotate */
//void camera::rotate( const vec3 &delta ) {
    //mat4::rotation( delta );

//}

/* camera::set_position */
void camera::set_position( const vec3 &position ) {
    this->position = position;
}

/* camera::set_position */
void camera::set_direction( const vec3 &direction ) {
    this->direction = direction;
    this->direction.normalize();
}

/* camera::set_up */
void camera::set_up( const vec3 &up ) {
    this->up = up;
    this->up.normalize();
}

/* camera::get_position */
const vec3 &camera::get_position() const {
    return position;
}

/* camera::get_direction */
const vec3 &camera::get_direction() const {
    return direction;
}

/* camera::get_up */
const vec3 &camera::get_up() {
    up = direction.cross( right );
    return up;
}

/* camera::get_right */
const vec3 &camera::get_right() {
    right = up.cross( direction );
    return right;
}

/* camera::operator() */
mat4 &camera::operator()() {
    right = up.cross( direction );
    up = direction.cross( right );
    out.x.x = right.x;
    out.x.y = right.y;
    out.x.z = right.z;
    out.x.w = 0.0;
    out.y.x = up.x;
    out.y.y = up.y;
    out.y.z = up.z;
    out.y.w = 0.0;
    out.z.x = direction.x;
    out.z.y = direction.y;
    out.z.z = direction.z;
    out.z.w = 0.0;
    out.w.x = 0.0;
    out.w.y = 0.0;
    out.w.z = 0.0;
    out.w.w = 1.0;
    out *= mat4::translation( -position );
    return out;
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

        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL );
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
    transform tr;
    camera cam;

    //cam.set_position( vec3( 0, -5, 0 ) );

    static float pi = 3.14159265358979;
    bool pause = false;
    bool change = true;
    bool prev = false;
    float stepSize = 0.1;


    while( appIsRun ) {
        if( prev != raw_input::is_key_pressed(0x5A) ) {
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
        prev = raw_input::is_key_pressed(0x5A);


        if( pause ) {
            timer.time_sec();
            w.process_messages();
            Sleep(1);
            r->display_frame();

            continue;
        }


        if( raw_input::is_key_pressed(VKRAW_W) ) {
            /*W key*/
            auto step = cam.get_direction() * stepSize;
            cam.move( step );
        }
        if( raw_input::is_key_pressed(VKRAW_A) ) {
            /*A key*/
            auto step = -cam.get_right() * stepSize;
            cam.move( step );
        }
        if( raw_input::is_key_pressed(VKRAW_S) ) {
            /*S key*/
             auto step = -cam.get_direction() * stepSize;
             cam.move( step );
        }
        if( raw_input::is_key_pressed(VKRAW_D) ) {
            /*D key*/
            auto step = cam.get_right() * stepSize;
            cam.move( step );
        }
        

        auto msec = timer.time_sec();
        w.process_messages();

        glViewport( 0, 0, w.get_size().width, w.get_size().height );


        fscale += 3.14159265358979 * msec * 0.25;
        fpos += 3.14159265358979 * msec * 0.3333333333333333333333;
        frot += 3.14159265358979 * msec *0.5;

        auto [sin, cos] = engine::math::sin_cos( fpos );
        tr.set_scale( 0.2,  0.2,  0.2 );
        tr.set_position( sin * 3.0, cos * 3.0, 10.0 );
        tr.set_rotation( frot, frot * 0.5, frot * 0.25 );


        //cam.move( vec3(cos * 0.1, sin * 0.1, 0) );
        auto perspective = mat4::perspective( pi / 180.0 * 60.0, w.get_aspect(), 1, 1000 );
        auto toShader = perspective * cam() * tr();


        glClear( GL_COLOR_BUFFER_BIT );
        /* send world matrix to shader */
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
