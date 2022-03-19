#pragma once
#include <core/types.hpp>
#include <core/string.hpp>
#include <core/vector.hpp>
#include <renderer/opengl/gl.h>
#include "uniform.h"

using namespace engine::core;

namespace engine {

class shader {
public:
/* index of the program in an array of shader objects or shader programs */
    typedef int         idprog;
    typedef int         idobj;
public:
    static idobj        load_vertex_shader( const string &name );
    static idobj        load_fragment_shader( const string &name );
    static idprog       link_program( const idobj vsh, const idobj fsh );
    static void         use_program( const idprog prog );

    bool                load( const string &vshName, const string &fshName );
    bool                is_loaded();
    void                use();
    uniform             get_uniform( const string &uniformName );
private:
    static idobj        load_shader_object( const string &name, GLenum type );
    static GLuint       load_shader( const string &name, GLenum type );

private:
    idobj               vertexShader{0};
    idobj               fragmentShader{0};
    idprog              program{0};

private:
/* OpenGL shader types: 
* GL_COMPUTE_SHADER,
* GL_VERTEX_SHADER, 
* GL_TESS_CONTROL_SHADER, 
* GL_TESS_EVALUATION_SHADER, 
* GL_GEOMETRY_SHADER,
* GL_FRAGMENT_SHADER.
*/
    struct shader_object {
            shader_object( const string &name, GLenum type, GLuint obj ) :
                    name{name}, type{type}, obj{obj} {}
        string      name;
        GLenum      type;
        GLuint      obj;
    };

    struct shader_program {
            shader_program( GLuint prog, idprog vsh, idprog fsh ) :
                    prog{prog}, vertexShaderId{vsh}, fragmentShaderId{fsh} {}
        GLuint      prog;               /* binded shader program */
        idobj       vertexShaderId;
        idobj       fragmentShaderId;
    };

    static core::vector<shader_object>  shaderObjects;
    static core::vector<shader_program> shaderPrograms;
};

/* shader::load_vertex_shader */
inline shader::idobj shader::load_vertex_shader( const string &name ) {
    return load_shader_object( name, GL_VERTEX_SHADER );
}

/* shader::load_fragment_shader */
inline shader::idobj shader::load_fragment_shader( const string &name ) {
    return load_shader_object( name, GL_FRAGMENT_SHADER );
}

/* shader::is_loaded */
inline bool shader::is_loaded() {
    return program != 0;
}

/* shader::is_loaded */
inline void shader::use() {
    shader::use_program( program );
}


} /* namespace engine */