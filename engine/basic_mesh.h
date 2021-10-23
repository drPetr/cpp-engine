#pragma once
#include <core/types.hpp>
#include <core/vector.hpp>
#include <core/assert.hpp>
#include "basic_mesh_present.h"
namespace engine {

const int BASIC_MESH_EXTRA_SIZE = 256;

/* basic_mesh container for mesh class */
class basic_mesh {
public:
                        basic_mesh( const present_vertex &presentVertex, const present_index &presentIndex );

    void                add_present_drawing( primitive_type type, int count, int offset );

    const present_vertex    &get_present_vertex();
    const present_index     &get_present_index();
    const present_drawing   &get_present_drawing();
    char                *get_vertex_ptr( int index );
    char                *get_index_ptr( int index );
    int                 get_vertices_number();
    int                 get_indices_number();
    unsigned int        get_restart_index();
    char                *get_extra_ptr();

protected:
    void                add_vertex( const char *vert, int size );
    void                add_vertices( const char *vert, int size, int num );
    void                add_index( const char *ind, int size );
    void                add_indices( const char *ind, int size, int num );

protected:
    core::vector<char>  vertices;   /* mesh vertices */
    core::vector<char>  indices;    /* mesh indices */
    present_vertex      presentVertex;  
    present_index       presentIndex;   /* index buffer bits 0, 1, 2 or 4 */
    present_drawing     presentDrawing;
    int                 indexSize{0};
    int                 verticesNum{0};
    int                 indicesNum{0};
    char                extra[BASIC_MESH_EXTRA_SIZE];
};



/* basic_mesh::add_present_drawing */
inline void basic_mesh::add_present_drawing( primitive_type type, int count, int offset ) {
    presentDrawing.add_present_drawing( type, count, offset );
    for( auto &i : extra ) {
        i = 0;
    }
}

/* basic_mesh::get_present_vertex */
inline const present_vertex &basic_mesh::get_present_vertex() {
    return presentVertex;
}

/* basic_mesh::get_present_index */
inline const present_index &basic_mesh::get_present_index() {
    return presentIndex;
}

/* basic_mesh::get_present_drawing */
inline const present_drawing &basic_mesh::get_present_drawing() {
    return presentDrawing;
}

/* basic_mesh::get_vertices_number */
inline int basic_mesh::get_vertices_number() {
    return verticesNum;
}

/* basic_mesh::get_indices_number */
inline int basic_mesh::get_indices_number() {
    return indicesNum;
}

/* basic_mesh::get_vertex_ptr */
inline char *basic_mesh::get_vertex_ptr( int index ) {
    assert( index >= 0 && index < verticesNum );
    return vertices.data() + index * presentVertex.vertexSize;
}

/* basic_mesh::get_index_ptr */
inline char *basic_mesh::get_index_ptr( int index ) {
    assert( presentIndex != PRESENT_INDEX_NO_INDEX );
    assert( index >= 0 && index < indicesNum );
    return indices.data() + index * indexSize;
}

/* basic_mesh::add_vertex */
inline void basic_mesh::add_vertex( const char *vert, int size ) {
    assert( size == presentVertex.vertexSize );
    add_vertices( vert, size, 1 );
}

/* basic_mesh::add_index */
inline void basic_mesh::add_index( const char *ind, int size ) {
    assert( presentIndex != PRESENT_INDEX_NO_INDEX );
    assert( size == indexSize );
    add_indices( ind, size, 1 );
}

/* basic_mesh::get_extra_ptr */
inline char *basic_mesh::get_extra_ptr() {
    return extra;
}

} /* namespace engine */