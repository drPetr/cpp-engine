#pragma once
#include "basic_mesh.h"
#include <core/math.h>
using namespace engine::math;
namespace engine {

/* type of vertex used in the mesh */
struct draw_vertex {
                draw_vertex() {} /* empty constructor */
                draw_vertex( const vec3 &pos, const vec2 &uv ) : 
                        pos{pos}, uv{uv} {}
    vec3        pos;
    vec2        uv;
    static present_vertex   get_present_vertex();
};

/* class mesh */
class mesh : public basic_mesh {
public:
                        mesh( present_index presentIndex = PRESENT_INDEX_16BITS ) : basic_mesh( draw_vertex::get_present_vertex(), presentIndex ) {}
    
    draw_vertex         &get_vertex( int index );
    unsigned int        &get_index( int index );
    void                add_vertex( const draw_vertex &vert );
    void                add_index( unsigned int ind );
};

} /* namespace engine */