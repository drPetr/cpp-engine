#include "basic_mesh_present.h"
#include <core/assert.hpp>
namespace engine {

/* present_vertex::vertex_attrib_type_size */
int present_vertex::vertex_attrib_type_size( const present_vertex_attrib vertexAttrib ) {
    switch( vertexAttrib ) {
        case PRESENT_VERTEX_ATTRIB_XYZ:
            return static_cast<int>( sizeof(float) );
        case PRESENT_VERTEX_ATTRIB_UV:
            return static_cast<int>( sizeof(float) );
        default:
            assert(0);
    }
    return 0;
}

/* present_vertex::vertex_attrib_elements_number */
int present_vertex::vertex_attrib_elements_number( const present_vertex_attrib vertexAttrib ) {
    switch( vertexAttrib ) {
        case PRESENT_VERTEX_ATTRIB_XYZ:
            return 3;
        case PRESENT_VERTEX_ATTRIB_UV:
            return 2;
        default:
            assert(0);
    }
    return 0;
}

/* present_vertex::add_present_attrib */
void present_vertex::add_present_attrib( const present_vertex_attrib presentAttrib ) {
    assert( numAttrib < PRESENT_VERTEX_ATTRIB_MAX_NUMBER );
    attributes[numAttrib].type = presentAttrib;
    attributes[numAttrib].offset = vertexSize;
    vertexSize += vertex_attrib_type_size(presentAttrib) *
            vertex_attrib_elements_number(presentAttrib);
    numAttrib++;
}

/* present_drawing::add_present_drawing */
void present_drawing::add_present_drawing( primitive_type type, int count, int offset ) {
    assert( numDraws < PRIMITIVE_TYPE_NUMBER );
    drawing[numDraws].type = type;
    drawing[numDraws].count = count;
    drawing[numDraws].offset = offset;
    numDraws++;
}


} /* namespace engine */