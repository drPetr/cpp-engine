#include "basic_mesh.h"
#include <core/string.hpp>

using namespace engine::core;

namespace engine {

/* basic_mesh::basic_mesh */
basic_mesh::basic_mesh( const present_vertex &presentVertex, const present_index &presentIndex ) {
    this->presentVertex = presentVertex;
    this->presentIndex = presentIndex;
    switch( presentIndex ) {
        case PRESENT_INDEX_32BITS:
            indexSize = 4;
            break;
        case PRESENT_INDEX_16BITS:
            indexSize = 2;
            break;
        case PRESENT_INDEX_8BITS:
            indexSize = 1;
            break;
        case PRESENT_INDEX_NO_INDEX:
            indexSize = 0;
            break;
        default:
            assert(0);
    }
}

/* basic_mesh::add_vertices */
void basic_mesh::add_vertices( const char *vert, int size, int num ) {
    assert( size == presentVertex.vertexSize );
    assert( num > 0 );
    size_t siz = num * size;

    //const string::iterator beg( const_cast<char*>(vert) );
    //const string::iterator end( const_cast<char*>(vert + siz) );
    vertices.insert( vertices.end(), vert, vert + siz);
    verticesNum += num;
}

/* basic_mesh::add_indices */
void basic_mesh::add_indices( const char *ind, int size, int num ) {
    assert( presentIndex != PRESENT_INDEX_NO_INDEX );
    assert( size == indexSize );
    size_t siz = num * size;
    //const string::iterator beg( const_cast<char*>(ind) );
    //const string::iterator end( const_cast<char*>(ind) + siz  );
    indices.insert( indices.end(), ind, ind + siz );
    indicesNum += num;
}

} /* namespace engine */