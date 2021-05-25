#include "mesh.h"
namespace engine {

/* draw_vertex::get_present_vertex */
present_vertex draw_vertex::get_present_vertex() {
    present_vertex present;
    present.add_present_attrib( PRESENT_VERTEX_ATTRIB_XYZ );
    assert( sizeof(draw_vertex) == present.vertexSize );
    return std::move(present);
}

/* mesh::get_vertex */
draw_vertex &mesh::get_vertex( int index ) {
    return *(reinterpret_cast<draw_vertex*>(get_vertex_ptr( index )));
}

/* mesh::get_index */
unsigned int &mesh::get_index( int index ) {
    return *(reinterpret_cast<unsigned int*>(get_index_ptr( index )));
}

/* mesh::add_vertex */
void mesh::add_vertex( const draw_vertex &vert ) {
    basic_mesh::add_vertex( reinterpret_cast<const char*>(&vert), 
            static_cast<int>(sizeof(draw_vertex)) );
}

/* mesh::add_index */
void mesh::add_index( unsigned int ind ) {
    basic_mesh::add_index( reinterpret_cast<const char*>(&ind), 
            static_cast<int>( indexSize) );
}

} /* namespace engine */