#pragma once
#include <core/types.hpp>
#include <core/vector.hpp>
#include <core/assert.hpp>
#include <core/string.hpp>

using namespace engine::core;

namespace engine {
namespace renderer {

enum image_format {
    IMAGE_FORMAT_AUTO,
    IMAGE_FORMAT_BMP,
    IMAGE_FORMAT_TGA,
    IMAGE_FORMAT_JPG,
    IMAGE_FORMAT_PNG
};

enum pixel_format {
    PIXEL_FORMAT_AUTO,
    PIXEL_FORMAT_GRAY8,
    PIXEL_FORMAT_BGR8,
    PIXEL_FORMAT_BGRA8,
    PIXEL_FORMAT_RGB8,
    PIXEL_FORMAT_RGBA8
};

class image {
public:
                    image() {}
                    image( int width, int height, const pixel_format fmt );
                    ~image();
                    
                    /* read/write */
    bool            load_from_file( const string &name, const pixel_format fmt = PIXEL_FORMAT_AUTO );
                    /* quality min 1 - minimum quality (maximum compression)
                    * max 100 - minimum compression */
    bool            save_to_file( const string &name, int quality = 95, const pixel_format fmt = PIXEL_FORMAT_AUTO, image_format imfmt = IMAGE_FORMAT_AUTO );

                    /* returns pointer to pixel */
    byte *          get_pixel_ptr( int x, int y );
    byte *          get_line_ptr( int y );
    
                    /* horizontal / vertical flip processing */
    void            horizontal_flip();
    void            vertical_flip();

                    /* returns true if image not loaded.
                    * Image loaded if width and height are not zero */
    bool            is_empty() const;

                    /* returns image size */
    int             get_width() const;
    int             get_height() const;
                    /* size in bytes of one line of pixels */
    int             get_stride() const;
    pixel_format    get_pixel_format() const;
                    /* returns bits per pixel */
    int             get_bpp() const;

                    /* reserve image memory */
    void            reserve( int width, int height, const pixel_format fmt );
    void            release();

    /* satatic methods */
    static int      pixel_format_to_bpp( const pixel_format fmt );
protected:
    bool            load_bmp( istream &is, pixel_format fmt );
    bool            save_bmp( ostream &os, pixel_format fmt );

    bool            load_tga( istream &is, pixel_format fmt );
                    /* if rle is ture then: 
                    * compress min 0 - maximum quality
                    * max 255 - maximum compression */
    bool            save_tga( ostream &os, pixel_format fmt, bool rle, byte compress );

    bool            load_jpg( istream &is, pixel_format fmt );
    bool            save_jpg( ostream &os, pixel_format fmt, int quality );

    bool            load_png( istream &is, pixel_format fmt );

    core::vector<byte>    data;           /* pixels data */
    int             width{0};
    int             height{0};
    int             stride{0};      /* image line pitch */
    int             bpp{0};         /* bits per pixel */
    pixel_format    fmt{PIXEL_FORMAT_AUTO};
};



/* image::is_empty */
inline bool image::is_empty() const {
    return width == 0 || height == 0;
}

/* image::get_pixel_ptr */
inline byte *image::get_pixel_ptr( int x, int y ) {
    assert( x >= 0 && y >= 0 );
    assert( x < width && y < height );
    assert( !is_empty() );
    size_t offset = y * stride + x * (bpp >> 3);
    return reinterpret_cast<byte*>(data.data() + offset);
}

/* image::get_pixel_ptr */
inline byte *image::get_line_ptr( int y ) {
    assert( y >= 0 && y < height );
    assert( !is_empty() );
    size_t offset = y * stride;
    return reinterpret_cast<byte*>(data.data() + offset);
}

/* image::get_width */
inline int image::get_width() const {
    return width;
}

/* image::get_height */
inline int image::get_height() const {
    return height;
}

/* image::get_stride */
inline int image::get_stride() const {
    return stride;
}

/* image::get_pixel_format */
inline pixel_format image::get_pixel_format() const {
    return fmt;
}

/* image::get_bpp */
inline int image::get_bpp() const {
    return bpp;
}

/* image::pixel_format_to_bpp */
inline int image::pixel_format_to_bpp( const pixel_format fmt ) {
    switch( fmt ) {
        case PIXEL_FORMAT_AUTO:
            return 0;
        case PIXEL_FORMAT_GRAY8:
            return 8;
        case PIXEL_FORMAT_RGB8:
        case PIXEL_FORMAT_BGR8:
            return 3 * 8;
        case PIXEL_FORMAT_BGRA8:
        case PIXEL_FORMAT_RGBA8:
            return 4 * 8;
    }
    assert(0);
    return 0;
}

} /* namespace renderer */
} /* namespace engine */