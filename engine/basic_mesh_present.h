#pragma once
namespace engine {

enum primitive_type {
    PRIMITIVE_TYPE_POINTS,              /* GL_POINTS */
    PRIMITIVE_TYPE_LINES,               /* GL_LINES */
    PRIMITIVE_TYPE_LINES_ADJACENCY,     /* GL_LINES_ADJACENCY */
    PRIMITIVE_TYPE_LINE_STRIP,          /* GL_LINE_STRIP */
    PRIMITIVE_TYPE_LINE_STRIP_ADJACENCY,/* GL_LINE_STRIP_ADJACENCY */
    PRIMITIVE_TYPE_LINE_LOOP,           /* GL_LINE_LOOP */
    PRIMITIVE_TYPE_TRIANGLES,           /* GL_TRIANGLES */
    PRIMITIVE_TYPE_TRIANGLES_ADJACENCY, /* GL_TRIANGLES_ADJACENCY */
    PRIMITIVE_TYPE_TRIANGLE_STRIP,      /* GL_TRIANGLE_STRIP */
    PRIMITIVE_TYPE_TRIANGLE_FAN,        /* GL_TRIANGLE_FAN */
    PRIMITIVE_TYPE_NUMBER = 10          /* total number of enum elements */
};

enum present_index {
    PRESENT_INDEX_32BITS,           /* unsigned int */
    PRESENT_INDEX_16BITS,           /* unsigned short */
    PRESENT_INDEX_8BITS,            /* unsigned char */
    PRESENT_INDEX_NO_INDEX          /* none */
};

enum present_vertex_attrib {
    PRESENT_VERTEX_ATTRIB_XYZ,      /* float 3 */
    PRESENT_VERTEX_ATTRIB_UV,       /* float 2 */
    PRESENT_VERTEX_ATTRIB_MAX_NUMBER = 16
};

struct present_vertex {
    struct {
        present_vertex_attrib   type{PRESENT_VERTEX_ATTRIB_MAX_NUMBER};
        int                     offset{0};  /* offset in bytes from the beginning of the vertex */
    } attributes[PRESENT_VERTEX_ATTRIB_MAX_NUMBER];
    int                 numAttrib{0};
    int                 vertexSize{0};

    static int          vertex_attrib_type_size( const present_vertex_attrib vertexAttrib );
    static int          vertex_attrib_elements_number( const present_vertex_attrib vertexAttrib );
    void                add_present_attrib( const present_vertex_attrib vertexAttrib );
};

struct present_drawing {
    struct {
        primitive_type  type{PRIMITIVE_TYPE_POINTS};
        int             count{0};   /* count of indices to drawing */
        int             offset{0};  /* offset (not in bytes) from the beginning of the indices array */  
    } drawing[PRIMITIVE_TYPE_NUMBER];
    int                 numDraws{0};

    void                add_present_drawing( primitive_type type, int count, int offset );
};


} /* namespace engine */