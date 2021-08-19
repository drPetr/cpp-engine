#include <iostream>
#include <windows.h>
#include <wingdi.h>
#include <cmath>
#include <memory>
#include <core/assert.h>
#include <core/timer.h>
#include <core/math.h>
#include <core/file_system.h>
#include <core/types.h>
#include <renderer/opengl/gl.h>
#include <engine/object3d_location.h>
#include <engine/mesh.h>
#include <renderer/shader.h>
#include <engine/controlled_camera.h>
#include <core/common.h>
#include <core/filesystem.h>
#include <engine/onoff_key.h>
#include <renderer/image.h>
#include <cstdlib>
#include <ctime>
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
    string          title = {"Window"};
    bool            isCreate = {false};
    whandle_t       handle = {nullptr};
    rect2d          rect = {{0, 0}, {1366, 768}};
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




struct mesh_binding {
    bool        isInit{false};
    GLuint      vbo{0};     /* GL vertex buffer object */
    GLuint      ibo{0};     /* GL index buffer object */
    GLuint      vao{0};     /* GL vertex array object */
    GLenum      indexType{0};
    GLuint      restartIndex{0};
};

class opengl_render {
public:
                        opengl_render( const whandle_t handle );
                        ~opengl_render();
    void                clear();  
    void                bind_mesh( basic_mesh &m );
    void                draw_mesh( basic_mesh &m );
    void                display_frame();

    static GLenum       primitive_type_to_gl_type( primitive_type type );
private:
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
    /* set default clear color */
    glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
}

/* opengl_render::~opengl_render */
opengl_render::~opengl_render() {
    ::wglMakeCurrent( NULL, NULL );
    ::wglDeleteContext( hrc );
    ::ReleaseDC( hWnd, hdc );
}

/* opengl_render::clear */
void opengl_render::clear() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/* opengl_render::bind_mesh */
void opengl_render::bind_mesh( basic_mesh &m ) {
    mesh_binding *b = reinterpret_cast<mesh_binding*>(m.get_extra_ptr());
    if( !(b->isInit) ) {
        assert( b->vbo == 0 );
        assert( b->ibo == 0 );
        assert( b->vao == 0 );
        const auto &vertPresent( m.get_present_vertex() );
        const auto &indPresent( m.get_present_index() );
        /* gen buffers and fill buffer data */
        glGenVertexArrays( 1, &b->vao );
        glBindVertexArray( b->vao );
        /* bind vertex buffer object */
        glGenBuffers( 1, &b->vbo );
        glBindBuffer( GL_ARRAY_BUFFER, b->vbo );
        glBufferData( GL_ARRAY_BUFFER, 
                vertPresent.vertexSize * m.get_vertices_number(), 
                m.get_vertex_ptr(0), GL_STATIC_DRAW );
        /* present vertex attributes */
        for( int i = 0; i < vertPresent.numAttrib; i++ ) {
            auto offset = vertPresent.attributes[i].offset;
            switch( vertPresent.attributes[i].type ) {
                case PRESENT_VERTEX_ATTRIB_XYZ:
                    glVertexAttribPointer( i, 3, GL_FLOAT, GL_FALSE, vertPresent.vertexSize, reinterpret_cast<void*>(offset) );
                    break;
                case PRESENT_VERTEX_ATTRIB_UV:
                    glVertexAttribPointer( i, 2, GL_FLOAT, GL_FALSE, vertPresent.vertexSize, reinterpret_cast<void*>(offset) );
                    break;
                case PRESENT_VERTEX_ATTRIB_MAX_NUMBER:
                    assert(0);
                    break;
            }
        }
        /* present index attributes */
        if( indPresent != PRESENT_INDEX_NO_INDEX ) {
            int indexBytes = 0;
            /* set gl index type */
            switch( indPresent ) {
                case PRESENT_INDEX_32BITS:
                    b->indexType = GL_UNSIGNED_INT;
                    b->restartIndex = 0xffffffff;
                    indexBytes = 4;
                    break;
                case PRESENT_INDEX_16BITS:
                    b->indexType = GL_UNSIGNED_SHORT;
                    b->restartIndex = 0xffff;
                    indexBytes = 2;
                    break;
                case PRESENT_INDEX_8BITS:
                    b->indexType = GL_UNSIGNED_BYTE;
                    b->restartIndex = 0xff;
                    indexBytes = 1;
                    break;
                default:
                    assert(0);
            }
            /* bind index buffer object */
            glGenBuffers( 1, &b->ibo );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, b->ibo );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
                    indexBytes * m.get_indices_number(), 
                    m.get_index_ptr(0), GL_STATIC_DRAW );
        } else {
            b->indexType = 0;
            b->restartIndex = 0;
        }
        glBindVertexArray( 0 );
        b->isInit = true;
    }
    assert( b->vao != 0 );
    glBindVertexArray( b->vao );
}

/* opengl_render::draw_mesh */
void opengl_render::draw_mesh( basic_mesh &m ) {
    bind_mesh( m );
    mesh_binding *b = reinterpret_cast<mesh_binding*>(m.get_extra_ptr());
    const auto &vertPresent( m.get_present_vertex() );
    for( int i = 0; i < vertPresent.numAttrib; i++ ) {
        glEnableVertexAttribArray(i);
    }
    /* draw calls */
    const auto &drawing = m.get_present_drawing();
    if( b->indexType != 0 ) {
        glPrimitiveRestartIndex( b->restartIndex );
        for( int i = 0; i < drawing.numDraws; i++ ) {
            auto mode = primitive_type_to_gl_type( drawing.drawing[i].type );
            glDrawElements( mode, drawing.drawing[i].count, b->indexType, 
                    reinterpret_cast<void*>(drawing.drawing[i].offset) );
        }
    } else {
        for( int i = 0; i < drawing.numDraws; i++ ) {
            auto mode = primitive_type_to_gl_type( drawing.drawing[i].type );
            glDrawArrays( mode, drawing.drawing[i].offset, drawing.drawing[i].count );
        }
    }
}

/* opengl_render::display_frame */
void opengl_render::display_frame() {
    ::SwapBuffers( hdc );
}

/* opengl_render::primitive_type_to_gl_type */
GLenum opengl_render::primitive_type_to_gl_type( primitive_type type ) {
    switch( type ) {
        case PRIMITIVE_TYPE_POINTS:
            return GL_POINTS;
        case PRIMITIVE_TYPE_LINES:
            return GL_LINES;
        case PRIMITIVE_TYPE_LINES_ADJACENCY:
            return GL_LINES_ADJACENCY;
        case PRIMITIVE_TYPE_LINE_STRIP:
            return GL_LINE_STRIP;
        case PRIMITIVE_TYPE_LINE_STRIP_ADJACENCY:
            return GL_LINE_STRIP_ADJACENCY;
        case PRIMITIVE_TYPE_LINE_LOOP:
            return GL_LINE_LOOP;
        case PRIMITIVE_TYPE_TRIANGLES:
            return GL_TRIANGLES;
        case PRIMITIVE_TYPE_TRIANGLES_ADJACENCY:
            return GL_TRIANGLES_ADJACENCY;
        case PRIMITIVE_TYPE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case PRIMITIVE_TYPE_TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        default:
            assert(0);
    }
    return 0;
}





} /* namespace engine */
using namespace engine;







float rand_float( float min, float max ) {
    float delta = max - min;
    float rnd = (rand() / (float)RAND_MAX) * delta;
    return min + rnd;
}


struct temp_s {
    object3d_location   loc;
    quat                qu;
};


 
#define __unused(v)   static_cast<void>(v)
int WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow ) {
    __unused(hInst); __unused(hPrevInst); __unused(lpCmdLine); __unused(nCmdShow);


    renderer::image img;
    core::timer tm;
    
    img.load_from_file( "1234.png" );
    /*tm.start();
    img.save_to_file( "image_out.jpg" );
    std::cout << "time: " << tm.get_elapsed_msec() << std::endl;*/

    //return 0;

    window w;
    w.align_center();
    w.create();
    w.show();

    opengl_render render( w.get_handle() );
    



    

    shader sh;
    sh.load( "shader.vsh", "shader.fsh" );
    auto uniWorld = sh.get_uniform( "gWorld" );
    auto uniTex = sh.get_uniform( "gTex" );


  
    const draw_vertex Verts[] = {
        {{0.000f,  0.000f,  1.000f},{0.0, 0.0}},
        {{0.894f,  0.000f,  0.447f},{0.0, 0.0}},
        {{0.276f,  0.851f,  0.447f},{0.0, 0.0}},
        {{-0.724f,  0.526f,  0.447f},{0.0, 0.0}},
        {{-0.724f, -0.526f,  0.447f},{0.0, 0.0}},
        {{0.276f, -0.851f,  0.447f},{0.0, 0.0}},
        {{0.724f,  0.526f, -0.447f},{0.0, 0.0}},
        {{-0.276f,  0.851f, -0.447f},{0.0, 0.0}},
        {{-0.894f,  0.000f, -0.447f},{0.0, 0.0}},
        {{-0.276f, -0.851f, -0.447f},{0.0, 0.0}},
        {{0.724f, -0.526f, -0.447f},{0.0, 0.0}},
        {{0.000f,  0.000f, -1.000f},{0.0, 0.0}}
    };

    const unsigned int Faces[] = {
        0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 1, 7, 6, 11, 
        8, 7, 11, 9, 8, 11, 10, 9, 11, 6, 10, 11, 6, 2, 1,
        7, 3, 2, 8, 4, 3, 9, 5, 4, 10, 1, 5, 6,  7, 2,
        7,  8, 3, 8,  9, 4, 9, 10, 5, 10, 6, 1
    };

    mesh sphere;

    for( const auto &v : Verts ) {
        sphere.add_vertex( v );
    }
    for( auto i : Faces ) {
        sphere.add_index( i );
    }
    sphere.add_present_drawing( 
                PRIMITIVE_TYPE_TRIANGLES, 
                sphere.get_indices_number(),
                0
    );

    

    const draw_vertex vert[] {
        {{-1.0, -1.0, -1.0},{0.0, 0.0}},
        {{-1.0,  1.0, -1.0},{0.0, 1.0}},
        {{ 1.0, -1.0, -1.0},{1.0, 0.0}},
        {{ 1.0,  1.0, -1.0},{1.0, 1.0}},
        {{ 1.0, -1.0,  1.0},{0.0, 0.0}},
        {{ 1.0,  1.0,  1.0},{0.0, 1.0}},
        {{-1.0, -1.0,  1.0},{1.0, 0.0}},
        {{-1.0,  1.0,  1.0},{1.0, 1.0}}
    };
    unsigned char Indices[] = {
        0,1,2,3,4,5,6,7,0xff,2,4,0,6,1,7,3,5
    };

    mesh cube(PRESENT_INDEX_8BITS);
    for( const auto &v : vert ) {
        cube.add_vertex( v );
    }
    for( auto i : Indices ) {
        cube.add_index( i );
    }
    cube.add_present_drawing( 
                PRIMITIVE_TYPE_TRIANGLE_STRIP, 
                cube.get_indices_number(), 0 );

    

    object3d_location loc;
    object3d_location loc2;
    object3d_location loc3;
    core::timer timer;
    controlled_camera cam( vec3(0,0,0), vec3(0,1,0), vec3(0,0,1) );
    cam.attach_input();
    //cam.set_position( vec3(0, -100, 0) );
    onoff_key pauseKey( VKRAW_BACK );
    pauseKey.attach_input();
    quat qu( vec3(1,2,3), pi / 123.0 );

    //glFrontFace( GL_CCW ); /* default */
    //glCullFace( GL_BACK ); /* default */
    glEnable( GL_CULL_FACE );
    glEnable( GL_PRIMITIVE_RESTART );
    glEnable( GL_DEPTH_TEST );
    raw_input::key currentKey = VKRAW_F1;

    /* texturing */
    GLuint textureObj;
    glGenTextures(1, &textureObj);
    glBindTexture(GL_TEXTURE_2D, textureObj);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 
            img.get_width(), img.get_height(),
            0, GL_RGBA, GL_UNSIGNED_BYTE, img.get_line_ptr(0) );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureObj);


    srand( time(NULL) );
    int locationsCount = 10000;
    temp_s *locations = new temp_s[locationsCount];
    for( int i = 0; i < locationsCount; i++ ) {
        float x, y, z;
        float s = rand_float(0.1, 4.0);
        locations[i].loc.set_scale( vec3(s,  s,  s) );
        x = rand_float(-500, 500);
        y = rand_float(-500, 500);
        z = rand_float(-500, 500);
        locations[i].loc.set_position( vec3(x, y, z) );
        x = rand_float(-500, 500);
        y = rand_float(-500, 500);
        z = rand_float(-500, 500);
        float del = rand_float(-500, 500);
        if( del == 0 ) {
            del = 100;
        }
        quat q( vec3(x,y,z), pi / del );
        locations[i].qu = q;
    }


    while( appIsRun ) {
        if( raw_input::is_key_pressed(VKRAW_ESCAPE) ) {
            appIsRun = false;
        }
        w.process_messages();

        if( raw_input::is_key_pressed(VKRAW_F1) && currentKey != VKRAW_F1 ) {
            currentKey = VKRAW_F1;
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            common::log() << "Polygon mode: FILL\n";
        }
        if( raw_input::is_key_pressed(VKRAW_F2) && currentKey != VKRAW_F2 ) {
            currentKey = VKRAW_F2;
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            common::log() << "Polygon mode: LINE\n";
        }
        if( raw_input::is_key_pressed(VKRAW_F3) && currentKey != VKRAW_F3 ) {
            currentKey = VKRAW_F3;
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
            common::log() << "Polygon mode: POINT\n";
        }
        if( raw_input::is_key_pressed(VKRAW_F5) && currentKey != VKRAW_F5 ) {
            currentKey = VKRAW_F5;
            glEnable( GL_CULL_FACE );
            common::log() << "Enable: CULL FACE\n";
        }
        if( raw_input::is_key_pressed(VKRAW_F6) && currentKey != VKRAW_F6 ) {
            currentKey = VKRAW_F6;
            glDisable( GL_CULL_FACE );
            common::log() << "Disable: CULL FACE\n";
        }
        
        if( pauseKey.is_active() ) {
            timer.time_sec();
            Sleep(16);
            render.display_frame();
            continue;
        }

        cam.update_movement();

        auto msec = timer.time_sec();
        __unused(msec);

        glViewport( 0, 0, w.get_size().width, w.get_size().height );

        loc.set_scale( vec3(0.5,  0.5,  0.5) );
        loc.set_position( vec3(0.0, 10.0, 0.0) );

        loc2.set_scale( vec3(1.0,  1.0,  1.0) );
        loc2.set_position( vec3(10.0, 0.0, 0.0) );
        loc2.rotate( qu );

        loc3.set_scale( vec3(2.0, 2.0, 2.0) );
        loc3.set_position( vec3(-5.0, 5.0, 0.0) );
        loc3.rotate( qu );

        cam.set_perspective_projection( pi / 3.0, w.get_aspect(), 0.1, 1000 );


        render.clear();
        
        auto toShader = cam() * loc();
        sh.use();
        uniWorld.set( toShader );
        glBindTexture(GL_TEXTURE_2D, textureObj);

        uniTex.set( GL_TEXTURE0 );


        uniWorld.set( toShader );
        render.draw_mesh( cube );

        for( int i = 0; i < locationsCount; i++ ) {
            toShader = cam() * locations[i].loc();
            uniWorld.set( toShader );
            render.draw_mesh( cube );
            locations[i].loc.rotate( locations[i].qu );
        }
        


        sh.use();
        toShader = cam() * loc2();
        uniWorld.set( toShader );
        render.draw_mesh( cube );

        toShader = cam() * loc3();
        uniWorld.set( toShader );
        render.draw_mesh( sphere );

        auto skip = static_cast<int>(1000.0 / 60.0 - timer.get_elapsed_msec());
        Sleep( skip > 0 ? skip : 0 );
        render.display_frame();
    }    
    
    delete[] locations;

    return 0;
}
