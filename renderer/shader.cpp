#include "shader.h"
#include <core/assert.hpp>
#include <core/common.hpp>
#include <core/filesystem.hpp>
namespace engine {

core::vector<shader::shader_object> shader::shaderObjects;
core::vector<shader::shader_program> shader::shaderPrograms;

/* shader::link_program */
shader::idprog shader::link_program( const idobj vsh, const idobj fsh ) {
    assert( vsh >= 0 && vsh <= static_cast<idobj>(shaderObjects.size()) );
    assert( fsh >= 0 && fsh <= static_cast<idobj>(shaderObjects.size()) );
    assert( vsh != fsh );
    assert( vsh != 0 || fsh != 0 );

    auto fail = false;
    auto program = glCreateProgram();
    if( !program ) {
        common::error() << "shader::link_program() error: glCreateProgram()" << std::endl;
        return 0;
    }
    if( vsh ) {
        shader_object &obj( shaderObjects[vsh - 1] );
        assert( obj.type == GL_VERTEX_SHADER );
        glAttachShader( program, obj.obj );
    }
    if( fsh ) {
        shader_object &obj( shaderObjects[fsh - 1] );
        assert( obj.type == GL_FRAGMENT_SHADER );
        glAttachShader( program, obj.obj );
    }
    glLinkProgram( program );
    /* check link status */
    GLint status;
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    if( !status ) {
        /* link program failed */
        char buffer[4096];
        glGetProgramInfoLog( program, sizeof(buffer), nullptr, buffer );
        common::error() << "shader::link_program() error: glLinkProgram()" << 
                std::endl << buffer << std::endl;
        fail = true;
    }
    /* validate program */
    glValidateProgram( program );
    glGetProgramiv( program, GL_VALIDATE_STATUS, &status );
    if( !status ) {
        /* validate program failed */
        char buffer[4096];
        glGetProgramInfoLog( program, sizeof(buffer), nullptr, buffer );
        common::error() << "shader::link_program() error: glValidateProgram()" << 
                std::endl << buffer << std::endl;
        fail = true;
    }
    /* detach shader objects */
    if( vsh ) {
        glDetachShader( program, vsh );
    }
    if( fsh ) {
        glDetachShader( program, fsh );
    }
    if( fail ) {
        /* linking the program failed */
        glDeleteProgram( program );
        return 0;
    }
    /* add program to list */
    shaderPrograms.push_back( shader_program( program, vsh, fsh ) );
    return static_cast<idprog>( shaderPrograms.size() + 65535 );
}

/* shader::use_program */
void shader::use_program( const idprog prog ) {
    assert( ((prog > 65535) && (prog <= static_cast<idprog>(shaderPrograms.size()) + 65535)) || prog == 0 );
    if( prog == 0 ) {
        glUseProgram( 0 );
        return;
    }
    auto &program( shaderPrograms[prog - 65535 - 1] );
    glUseProgram( program.prog );
}

/* shader::load */
bool shader::load( const string &vshName, const string &fshName ) {
    assert( (vshName != "") || (fshName != "") );
    assert( !is_loaded() );
    if( vshName != "" ) {
        vertexShader = shader::load_vertex_shader( vshName );
    }
    if( fshName != "" ) {
        fragmentShader = shader::load_fragment_shader( fshName );
    }
    program = shader::link_program( vertexShader, fragmentShader );
    return is_loaded();
}

/* shader::get_uniform */
uniform shader::get_uniform( const string &uniformName ) {
    auto glprog = shaderPrograms[program - 65535 - 1].prog;
    auto var = glGetUniformLocation( glprog, uniformName.c_str() );
    return std::move( uniform(var) );
}

/* shader::load_shader_object */
shader::idobj shader::load_shader_object( const string &name, GLenum type ) {
    auto shader = load_shader( name, type );
    if( !shader ) {
        return 0;
    }
    shaderObjects.push_back( shader_object( name, type, shader ) );
    return static_cast<idobj>(shaderObjects.size());
}

/* shader::load_shader */
GLuint shader::load_shader( const string &name, GLenum type ) {
    /* load shader text */
    auto [success, contents] = core::filesystem::read_contents( name );
    if( !success ) {
        common::error() << "shader::load_shader() error: file '" << name << "' not found." << std::endl;
        return 0;
    }

    /* create shader object */
    auto shader = glCreateShader( type );
    if( shader == 0 ) {
        common::error() << "shader::load_shader() error: glCreateShader() returns 0" << std::endl;
        return 0;
    }
    /* compile shader */
    auto *shaderText = contents.c_str();
    auto shaderTextLength = static_cast<GLint>( contents.length() );
    glShaderSource( shader, 1, &shaderText, &shaderTextLength );
    glCompileShader( shader );
    /* check compile status */
    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    if( !status ) {
        /* compilation failed */
        char buffer[4096];
        glGetShaderInfoLog( shader, sizeof(buffer), nullptr, buffer );   
        common::error() << "shader::load_shader() error: glCompileShader()" << 
                std::endl << buffer << std::endl;
        glDeleteShader( shader );
        return 0;
    }
    return shader;
}

}