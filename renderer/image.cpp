#include "image.h"
#include <core/common.hpp>
#include <core/filesystem.hpp>
#include <core/math.hpp>
extern "C" {
#include <jpeg-6b/jpeglib.h>
#include <jpeg-6b/jdatarw.h>
#include <lpng1637/png.h>
#include <zlib-1.2.11/zlib.h>
}
/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

#include <setjmp.h>

namespace engine {
namespace renderer {

typedef void(*fnPxcvtFunc)(const byte*,byte*);
typedef int(*fnPxcmpFunc)(const byte*,const byte*);

/* image::image */
image::image( int width, int height, const pixel_format fmt ) {
    reserve( width, height, fmt );
}

/* image::~image */
image::~image() {
    release();
}

/* image::load_from_file */
bool image::load_from_file( const string &name, pixel_format fmt ) {
    assert( is_empty() );
    ifstream file( filesystem::open_read(name) );
    if( file.is_open() ) {
        if( load_bmp( file, fmt ) ) {
            return true;
        } else if( load_png( file, fmt ) ) {
            return true;
        } else if( load_tga( file, fmt ) ) {
            return true;
        } else if( load_jpg( file, fmt ) ) {
            return true;
        }
    }
    return false;
}

/* image::save_to_file */
bool image::save_to_file( const string &name, int quality, const pixel_format fmt, image_format imfmt ) {
    assert( !is_empty() );
    if( imfmt == IMAGE_FORMAT_AUTO ) {
        if( name.length() < 5 ) {
            common::error() << "image::save_to_file() error: wrong file format" << std::endl;
            return false;
        }
        auto fileFmt = name.substr( name.length() - 5, 5 );
        for( auto &c : fileFmt ) {
            c = ::tolower(c);
        }
        if( fileFmt.compare( 1, 4, ".tga" ) == 0 ) {
            imfmt = IMAGE_FORMAT_TGA;
        } else if( fileFmt.compare( 1, 4, ".bmp" ) == 0 ) {
            imfmt = IMAGE_FORMAT_BMP;
        } else if( (fileFmt.compare( 1, 4, ".jpg" ) == 0) ||
                 (fileFmt.compare( 0, 5, ".jpeg" ) == 0) ) {
            imfmt = IMAGE_FORMAT_JPG;
        } else if( fileFmt.compare( 1, 4, ".png" ) == 0 ) {
            imfmt = IMAGE_FORMAT_PNG;
        } else {
            common::error() << "image::save_to_file() error: wrong file format" << std::endl;
            return false;
        }
    }

    if( quality > 100 ) {
        quality = 100;
    } else if( quality < 1 ) {
        quality = 1;
    }

    ofstream file( filesystem::open_write(name) );
    if( file.is_open() ) {
        switch( imfmt ) {
            case IMAGE_FORMAT_BMP:
                return save_bmp( file, fmt );
            case IMAGE_FORMAT_TGA: {
                bool useRle = quality < 100;
                byte compress = static_cast<byte>(100 - quality);
                compress /= 10;
                return save_tga( file, fmt, useRle, compress );
            }
            case IMAGE_FORMAT_JPG:
                return save_jpg( file, fmt, quality );

            default:
                assert( 0 && "Unknown file format" );
        }
    }
    return false;
}

/* image::horizontal_flip */
void image::horizontal_flip() {
    assert( !is_empty() );

    int pxSize = get_bpp() >> 3;
    for( int i = 0 ; i < height ; i++ ) {
        byte *data = this->data.data() + i * stride;
        for( int j = 0 ; j < width / 2 ; j++ ) {
            for( int k = 0; k < pxSize; k++ ) {
                auto tmp = *(data + j * pxSize + k);
                *(data + j * pxSize + k) = *(data + stride - (j - 1) * pxSize + k);
                *(data + stride - (j - 1) * pxSize + k) = tmp;
            }
        }
    }
}

/* image::vertical_flip */
void image::vertical_flip() {
    assert( !is_empty() );

    int pxSize = get_bpp() >> 3;
    for( int i = 0; i < height / 2; i++ ) {
        assert( i < height / 2 );
        byte *a = this->data.data() + stride * i;
        byte *b = this->data.data() + stride * (height - i - 1);
        for( int j = 0; j < width * pxSize; j++, a++, b++ ) {
            auto tmp = *a;
            *a = *b;
            *b = tmp;
        }
    }
}

/* image::reserve */
void image::reserve( int width, int height, const pixel_format fmt ) {
    assert( width > 0 );
    assert( height > 0 );
    assert( fmt != PIXEL_FORMAT_AUTO );

    this->fmt = fmt;
    bpp = pixel_format_to_bpp( fmt );
    this->width = width;
    this->height = height;
    stride = (bpp >> 3) * width;
    data.reserve( stride * height );
}

/* image::release */
void image::release() {
    data.clear();
    width = 0;
    height = 0;
    stride = 0;
    bpp = 0;
    fmt = PIXEL_FORMAT_AUTO;
}

/*
================================================
            BMP (bitmap) file format
================================================
*/

/* An uncompressed format. */
#define BITMAP_RGB      0L

/* A run-length encoded (RLE) format for bitmaps with 8 bpp. The compression 
* format is a 2-byte format consisting of a count byte followed by a byte 
* containing a color index. For more information, see Bitmap Compression */
#define BITMAP_RLE8     1L

/* An RLE format for bitmaps with 4 bpp. The compression format is a 2-byte 
* format consisting of a count byte followed by two word-length color indexes. 
* For more information, see Bitmap Compression */
#define BITMAP_RLE4     2L

/* Specifies that the bitmap is not compressed and that the color table consists
* of three DWORD color masks that specify the red, green, and blue components, 
* respectively, of each pixel. This is valid when used with 16- and 32-bpp\
* bitmaps */
#define BITMAP_BITFIELDS 3L 

/* Indicates that the image is a JPEG image */
#define BITMAP_JPEG     4L 

/* Indicates that the image is a PNG image */
#define BITMAP_PNG      5L 

#define BITMAP_SIGNATURE         0x4d42

#pragma pack(push, 1)
struct bitmap_file_header { /* 14 bytes */
    word        signature;  /* The file signature; must be BM */
    dword       size;       /* The size, in bytes, of the bitmap file */
    word        reserved1;  /* Reserved; must be zero  */
    word        reserved2;  /* Reserved; must be zero  */
    dword       offset;     /* The offset, in bytes, from the beginning of the bitmapHeader_t
                            structure to the bitmap bits */
};

struct bitmap_info_header { /* 40 bytes */
    dword       size;       /* The number of bytes required by the structure */
    int         width;      /* The width of the bitmap, in pixels */
    int         height;     /* The height of the bitmap, in pixels. If biHeight is positive, the 
                            * bitmap is a bottom-up DIB and its origin is the lower-left corner.
                            * If biHeight is negative, the bitmap is a top-down DIB and its
                            * origin is the upper-left corner */
    word        planes;     /* The number of planes for the target device. This value must be 
                            * set to 1 */
    word        bitCount;   /* The number of bits-per-pixel. The biBitCount member of the 
                            * BITMAPINFOHEADER structure determines the number of bits that define
                            * each pixel and the maximum number of colors in the bitmap. This 
                            * member must be one of the following values */
    dword       compression;/*  */ 
    dword       imageSize;  /* The size, in bytes, of the image. This may be set to zero for 
                            * BI_RGB bitmaps */
    int         xPxPerMeter;/* The horizontal resolution, in pixels-per-meter, of the target 
                            * device for the bitmap. An application can use this value to select
                            * a bitmap from a resource group that best matches the characteristics
                            * of the current device  */
    int         yPxPerMeter;/* The vertical resolution, in pixels-per-meter, of the target device
                            * for the bitmap */
    dword       colorUsed;  /* The number of color indexes in the color table that are actually
                            * used by the bitmap. If this value is zero, the bitmap uses the
                            * maximum number of colors corresponding to the value of the biBitCount
                            * member for the compression mode specified by biCompression */
    dword       colorImportant;/* The number of color indexes that are required for displaying 
                            * the bitmap. If this value is zero, all colors are required */
};

struct bitmap_rgba {   /* 4 bytes */
    byte        b;      /* Blue color */
    byte        g;      /* Green color */
    byte        r;      /* Red color */
    byte        a;      /* Alpha color */
};

struct bitmap_rgb {     /* 3 bytes */
    byte        b;      /* Blue color */
    byte        g;      /* Green color */
    byte        r;      /* Red color */
};

struct rgba32 {
    dword       r;
    dword       g;
    dword       b;
    dword       a;
};
#pragma pack(pop)

/* pixel covert functions */
static void pxcvt_bgr8_rgb8( const byte *from, byte *to ) {
    to[0] = from[2];
    to[1] = from[1];
    to[2] = from[0];
}

static void pxcvt_bgr8_rgba8( const byte *from, byte *to ) {
    to[0] = from[2];
    to[1] = from[1];
    to[2] = from[0];
    to[3] = 0;
}

static void pxcvt_bgr8_gray8( const byte *from, byte *to ) {
    to[0] = (from[2] + from[1] + from[0]) / 3;
}

static void pxcvt_gray8_bgr8( const byte *from, byte *to ) {
    to[0] = from[0];
    to[1] = from[0];
    to[2] = from[0];
}

static void pxcvt_gray8_bgra8( const byte *from, byte *to ) {
    to[0] = from[0];
    to[1] = from[0];
    to[2] = from[0];
    to[3] = 0;
}

static void pxcvt_bgra8_rgba8( const byte *from, byte *to ) {
    to[0] = from[2];
    to[1] = from[1];
    to[2] = from[0];
    to[3] = from[3];
}

static void pxcpy1( const byte *from, byte *to ) {
    to[0] = from[0];
}

static void pxcpy3( const byte *from, byte *to ) {
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
}

static void pxcpy3_expand1( const byte *from, byte *to ) {
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
    to[3] = 0;
}

static void pxcpy4( const byte *from, byte *to ) {
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
    to[3] = from[3];
}

/* get_pxcvt_func */
static fnPxcvtFunc get_pxcvt_func( pixel_format from, pixel_format to ) {
    assert( from != PIXEL_FORMAT_AUTO );
    assert( to != PIXEL_FORMAT_AUTO );
    switch(from) {
        case PIXEL_FORMAT_GRAY8:
            switch(to) {
                case PIXEL_FORMAT_GRAY8:    return pxcpy1;
                case PIXEL_FORMAT_BGR8:     return pxcvt_gray8_bgr8;
                case PIXEL_FORMAT_BGRA8:    return pxcvt_gray8_bgra8;
                case PIXEL_FORMAT_RGB8:     return pxcvt_gray8_bgr8;
                case PIXEL_FORMAT_RGBA8:    return pxcvt_gray8_bgra8;
                case PIXEL_FORMAT_AUTO:     break;
            }
            break;
        case PIXEL_FORMAT_BGR8:
            switch(to) {
                case PIXEL_FORMAT_GRAY8:    return pxcvt_bgr8_gray8;
                case PIXEL_FORMAT_BGR8:     return pxcpy3;
                case PIXEL_FORMAT_BGRA8:    return pxcpy3_expand1;
                case PIXEL_FORMAT_RGB8:     return pxcvt_bgr8_rgb8;
                case PIXEL_FORMAT_RGBA8:    return pxcvt_bgr8_rgba8;
                case PIXEL_FORMAT_AUTO:     break;
            }
            break;
        case PIXEL_FORMAT_BGRA8:
            switch(to) {
                case PIXEL_FORMAT_GRAY8:    return pxcvt_bgr8_gray8;
                case PIXEL_FORMAT_BGR8:     return pxcpy3;
                case PIXEL_FORMAT_BGRA8:    return pxcpy4;
                case PIXEL_FORMAT_RGB8:     return pxcvt_bgr8_rgb8;
                case PIXEL_FORMAT_RGBA8:    return pxcvt_bgra8_rgba8;
                case PIXEL_FORMAT_AUTO:     break;
            }
            break;
        case PIXEL_FORMAT_RGB8:
            switch(to) {
                case PIXEL_FORMAT_GRAY8:    return pxcvt_bgr8_gray8;
                case PIXEL_FORMAT_BGR8:     return pxcvt_bgr8_rgb8;
                case PIXEL_FORMAT_BGRA8:    return pxcvt_bgr8_rgba8;
                case PIXEL_FORMAT_RGB8:     return pxcpy3;
                case PIXEL_FORMAT_RGBA8:    return pxcpy3_expand1;
                case PIXEL_FORMAT_AUTO:     break;
            }
            break;
        case PIXEL_FORMAT_RGBA8:
            switch(to) {
                case PIXEL_FORMAT_GRAY8:    return pxcvt_bgr8_gray8;
                case PIXEL_FORMAT_BGR8:     return pxcvt_bgr8_rgb8;
                case PIXEL_FORMAT_BGRA8:    return pxcvt_bgra8_rgba8;
                case PIXEL_FORMAT_RGB8:     return pxcpy3;
                case PIXEL_FORMAT_RGBA8:    return pxcpy4;
                case PIXEL_FORMAT_AUTO:     break;
            }
            break;
        case PIXEL_FORMAT_AUTO: break;
    }
    assert(0);
    return nullptr;
}

/* pixel comparer */
static int pxcmp_8( const byte *a, const byte *b ) {
    return static_cast<int>(*a - *b);
}

static int pxcmp_24( const byte *a, const byte *b ) {
    int cmp = 0;
    for( int i = 0; i < 3; i++ ) {
        int c = a[i] - b[i];
        if( core::math::abs(c) > core::math::abs(cmp) ) {
            cmp = c;
        }
    }
    return cmp;
}

static int pxcmp_32( const byte *a, const byte *b ) {
    int cmp = 0;
    for( int i = 0; i < 4; i++ ) {
        int c = a[i] - b[i];
        if( core::math::abs(c) > core::math::abs(cmp) ) {
            cmp = c;
        }
    }
    return cmp;
}

/* get_pxcmp_func */
static fnPxcmpFunc get_pxcmp_func( pixel_format fmt ) {
    switch(fmt) {
        case PIXEL_FORMAT_GRAY8:    return pxcmp_8;
        case PIXEL_FORMAT_BGR8:     return pxcmp_24;
        case PIXEL_FORMAT_BGRA8:    return pxcmp_32;
        case PIXEL_FORMAT_RGB8:     return pxcmp_24;
        case PIXEL_FORMAT_RGBA8:    return pxcmp_32;
        case PIXEL_FORMAT_AUTO:     break;
    }
    assert(0);
    return nullptr;
}

/* image::load_bmp */
bool image::load_bmp( istream &is, pixel_format fmt ) {
    bitmap_file_header header;
    bitmap_info_header info;
    
    auto pos = is.tellg();
    /* Check for correct data */
    if( !is.read( reinterpret_cast<char*>(&header), sizeof(header) ) ) {
        is.seekg( pos );
        return false;
    }
    if( !is.read( reinterpret_cast<char*>(&info), sizeof(info) ) ) {
        is.seekg( pos );
        return false;
    }
    if( !is.good() ) {
        is.seekg( pos );
        return false;
    }
    if( header.signature != BITMAP_SIGNATURE ) {
        is.seekg( pos );
        return false;
    }
    if( info.size != sizeof(info) ) {
        common::error() << "image::load_bmp() error: info.size != sizeof(info)" << std::endl;
        is.seekg( pos );
        return false;
    }
    /* check supported formats */
    if( auto b = info.bitCount; !(b == 8 || b == 16 || b == 24 || b == 32) ) {
        common::error() << "image::load_bmp() error: unsupported format info.bitCount: '" 
            << info.bitCount << "'" << std::endl;
        is.seekg( pos );
        return false;
    }
    if( info.width <= 0 || info.height <= 0 ) {
        common::error() << "image::load_bmp() error: wrong image size" << std::endl;
        is.seekg( pos );
        return false;
    }
    if( info.compression != BITMAP_RGB ) {
        common::error() << "image::load_bmp() error: unsupported compression: '"
                << info.compression << "'" << std::endl;
        is.seekg( pos );
        return false;
    }

    /* Read bitmap palette */
    if( !info.colorUsed ) {
        info.colorUsed = info.bitCount == 8 ? 256 : 0;
    }
    if( info.colorUsed ) {
        for( int i = 0; i < static_cast<int>(info.colorUsed); i++ ) {
            if( bitmap_rgba quad; !is.read( reinterpret_cast<char*>(&quad), sizeof(quad) ) ) {
                common::error() << "image::load_bmp() error: reading error (read quad)" << std::endl;
                is.seekg( pos );
                return false;
            }
            if( !is.good() ) {
                common::error() << "image::load_bmp() error: reading error (is good)" << std::endl;
                is.seekg( pos );
                return false;
            }
        }
    }
    
    /* Convert bitmap pixel format to iner pixel format */
    pixel_format bmpFmt = PIXEL_FORMAT_AUTO;
    switch( info.bitCount ) {
        case 8:
            assert(0);
            break;
        case 16:
            assert(0);
            break;
        case 24:
             bmpFmt = PIXEL_FORMAT_BGR8;
             break;
        case 32:
            bmpFmt = PIXEL_FORMAT_BGRA8;
            break;
        default:
            assert(0);
    }

    /* check pixel format and reserve image */
    if( fmt == PIXEL_FORMAT_AUTO ) {
        fmt = bmpFmt;
    }
    reserve( info.width, info.height, fmt );

    int bmpPxSize = info.bitCount >> 3;
    int pxSize = get_bpp() >> 3;
    int stride = ((info.bitCount * info.width + 31) & ~31) >> 3;
    int rowSize = ((info.bitCount * info.width + 7) & ~7) >> 3;
    int dataPadding = stride - rowSize;
    assert( dataPadding == 0 );
    byte buffer[256];
    //byte bufferTo[256];
    fnPxcvtFunc cvt = get_pxcvt_func( bmpFmt, fmt );

    is.seekg( header.offset, std::ios_base::beg );
    for( int i = 0; i < info.height; i++ ) {
        /* Set pointer for copying data */
        byte *data = this->data.data() + this->stride * (info.height - i - 1);
        for( int j = 0; j < info.width; j++ ) {
            /* Read one pixel data from file */
            if( !is.read( reinterpret_cast<char*>(buffer), bmpPxSize ) ) {
                release();
                common::error() << "image::load_bmp() error: reading error (read data)" << std::endl;
                is.seekg( pos );
                return false;
            }
            cvt( buffer, data );
            data += pxSize;
            //this->data.insert( this->data.end(), bufferTo, bufferTo + bmpPxSize );
        }
        if( dataPadding ) {
            is.seekg( dataPadding, std::ios_base::cur );
        }
    }
    return true;
}

/* image::save_bmp */
bool image::save_bmp( ostream &os, pixel_format fmt ) {
    bitmap_file_header header;
    bitmap_info_header info;
    
    /* if needed then get best pixel format to save */
    if( fmt == PIXEL_FORMAT_AUTO ) {
        switch( this->fmt ) {
            case PIXEL_FORMAT_RGB8:
            case PIXEL_FORMAT_BGR8:
                fmt = PIXEL_FORMAT_BGR8;
                break;
            case PIXEL_FORMAT_RGBA8:
            case PIXEL_FORMAT_BGRA8:
                fmt = PIXEL_FORMAT_BGRA8;
                break;
            default:
                assert(0);
                break;
        }
    }
    
    dword bmpBpp = pixel_format_to_bpp( fmt );
    dword offset = static_cast<dword>( sizeof(header) + sizeof(info) );
    dword stride = ((bmpBpp * this->width + 31) & ~31) >> 3;
    int rowSize = ((this->width * bmpBpp + 7) & ~7) >> 3;
    int dataPadding = stride - rowSize;
    assert( dataPadding == 0 );
    
    /* Set bitmap file header */
    header.signature = BITMAP_SIGNATURE;
    header.size = offset + stride * this->height;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offset = offset;
    /* Set bitmap info header */
    info.size = sizeof(info);
    info.width = this->width;
    info.height = this->height;
    info.planes = 1;
    info.bitCount = bmpBpp;
    info.compression = BITMAP_RGB;
    info.imageSize = stride * this->height;
    info.xPxPerMeter = 0;
    info.yPxPerMeter = 0;
    info.colorUsed = 0;
    info.colorImportant = 0;

    fnPxcvtFunc cvt = get_pxcvt_func( this->fmt, fmt );
    int bmpPxSize = bmpBpp >> 3;
    int pxSize = get_bpp() >> 3;
    byte buffer[256];
    
    /* Write file and info headers */
    if( !os.write( reinterpret_cast<char*>(&header), sizeof(header) ) ) {
        common::error() << "image::save_bmp() error: writing error (header)" << std::endl;
        goto goRetErr;
    }
    if( !os.write( reinterpret_cast<char*>(&info), sizeof(info) ) ) {
        common::error() << "image::save_bmp() error: writing error (info)" << std::endl;
        goto goRetErr;
    }
    /* Write bitmap data */
    for( int i = 0; i < this->height; i++ ) {
        byte *data = this->data.data() + this->stride * (this->height - i - 1);
        for( int j = 0; j < this->width; j++ ) {
            cvt( data, buffer );
            if( !os.write( reinterpret_cast<char*>(buffer), bmpPxSize ) ) {
                common::error() << "image::save_bmp() error: writing error (info)" << std::endl;
                goto goRetErr;
            }
            data += pxSize;
        }
        if( dataPadding ) {
            if( dword zero = 0; !os.write( reinterpret_cast<char*>(&zero), dataPadding ) ) {
                common::error() << "image::save_bmp() error: writing error (dataPadding)" << std::endl;
                goto goRetErr;
            }
        }
    }
    return true;

goRetErr:
    return false;
}



/*
================================================
                TGA file format
================================================
*/
#pragma pack(push, 1)
typedef struct _TargaHeader {
    unsigned char   id_length, colormap_type, image_type;
    unsigned short  colormap_index, colormap_length;
    unsigned char   colormap_size;
    unsigned short  x_origin, y_origin, width, height;
    unsigned char   pixel_size, attributes;
} TargaHeader;

/* http://www.paulbourke.net/dataformats/tga/ */
struct targa_header {
/* idLength:
* is the length of a string located located after the header. */
    byte        idLength;
    byte        colormapType;
/* dataType:
* 0  - No image data included.
* 1  - Uncompressed, color-mapped images.
* 2  - Uncompressed, RGB images.
* 3  - Uncompressed, black and white images.
* 9  - Runlength encoded color-mapped images.
* 10 - Runlength encoded RGB images.
* 11 - Runlength encoded black and white images.
* 32 - Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
* 33 - Compressed color-mapped data, using Huffman, Delta, and runlength encoding.  
*      4-pass quadtree-type process. */
    byte        dataType;
    word        colormapIndex;
    word        colormapLength;
    byte        colormapSize;
    word        xOrigin;
    word        yOrigin;
    word        width;
    word        height;
/* bpp:
* The bitsperpixel specifies the size of each colour value. When 24 or 32 the 
* normal conventions apply. For 16 bits each colour component is stored as 
* 5 bits and the remaining bit is a binary alpha value. The colour components 
* are converted into single byte components by simply shifting each component 
* up by 3 bits (multiply by 8). */
    byte        bpp;
    byte        attrib;
};
#pragma pack(pop)

/* tga_16_to_bgra8 */
inline static void tga_16_to_bgra8( byte *pixels ) {
    byte b =  pixels[0] & 0b11111000;
    byte g = (pixels[0] & 0b00000111) << 5 | (pixels[1] & 0b11000000) >> 3;
    byte r = (pixels[1] & 0b00111110) << 2;
    byte a = (pixels[1] & 0x00000001) << 7;
    pixels[0] = b;
    pixels[1] = g;
    pixels[2] = r;
    pixels[3] = a;
}

/* load_tga_read_rle */
static bool load_tga_read_rle( istream &is, targa_header &header, byte *dataPtr, int pxSize, fnPxcvtFunc cvt, int stride ) {
    byte buffer[256];
    int tgaPxSize = header.bpp >> 3;
    /* Runlength encoded data (RLE) */
    for( int i = 0; i < header.height; ) {
        byte *data = dataPtr + stride * (header.height - i - 1);
        for( int j = 0; j < header.width; ) {
            byte chunkHeader;
            /* read one byte (RLE packet header) */
            if( !is.read( reinterpret_cast<char*>(&chunkHeader), 1 ) ) {
                common::error() << "image::load_tga() error: reading error (read RLE packet header)" << std::endl;
                return false;
            }
            byte chunkSize = (chunkHeader & 0x7f) + 1;
            if( chunkHeader & 0x80 ) {
                /* it is RLE */
                /* Read pixel data */
                if( !is.read( reinterpret_cast<char*>(buffer), tgaPxSize ) ) {
                    common::error() << "image::load_tga() error: reading error (read packed RLE data)" << std::endl;
                    return false;
                }
                if( header.bpp == 16 ) {
                    /* firstly convert to supported pixel format */
                    tga_16_to_bgra8( buffer );
                }
                for( int k = 0; k < chunkSize; k++ ) {
                    cvt( buffer, data );
                    if( ++j == header.width ) {
                        j = 0;
                        if( ++i == header.height ) {
                            goto goOut;
                        }
                        data = dataPtr + stride * (header.height - i - 1);
                    } else {
                        data += pxSize;
                    }
                }
            } else {
                for( int k = 0; k < chunkSize; k++ ) {
                    /* Read pixel data */
                    if( !is.read( reinterpret_cast<char*>(buffer), tgaPxSize ) ) {
                        common::error() << "image::load_tga() error: reading error (read packed data)" << std::endl;
                        return false;
                    }
                    if( header.bpp == 16 ) {
                        /* firstly convert to supported pixel format */
                        tga_16_to_bgra8( buffer );
                    }
                    cvt( buffer, data );
                    if( ++j == header.width ) {
                        j = 0;
                        if( ++i == header.height ) {
                            goto goOut;
                        }
                        data = dataPtr + stride * (header.height - i - 1);
                    } else {
                        data += pxSize;
                    }
                }
            }
        }
    }
goOut:
    return true;
}

/* image::load_tga */
bool image::load_tga( istream &is, pixel_format fmt ) {
    targa_header header;
    byte buffer[256];

    auto pos = is.tellg();
    /* Check for correct data */
    if( !is.read( reinterpret_cast<char*>(&header), sizeof(header) ) ) {
        is.seekg( pos );
        return false;
    }
    if( !is.good() ) {
        is.seekg( pos );
        return false;
    }
    /* is this exactly a tga file? check this */
    if( auto t = header.colormapType; !(t == 0 || t == 1) ) {
        /* this file not a TGA */
        is.seekg( pos );
        return false;
    }
    if( auto t = header.dataType; !(t == 0 || t == 1 || t == 2 || t == 3 || 
            t == 9 || t == 10 || t == 11) ) {
        /* this file not a TGA */
        is.seekg( pos );
        return false;
    }
    if( auto t = header.bpp; !(t == 8 || t == 16 || t == 24 || t == 32) )  {
        /* this file not a TGA */
        is.seekg( pos );
        return false;
    }
    /* checks supported formats */
    if( header.colormapType != 0 || header.dataType == 1 || header.dataType == 9 ) {
        common::error() << "image::load_tga() error: colormaps not supported" << std::endl;
        is.seekg( pos );
        return false;
    }
    if( header.idLength != 0 ) {
        if( !is.read( reinterpret_cast<char*>(buffer), header.idLength ) ) {
            is.seekg( pos );
            common::error() << "image::load_tga() error: reading error (header.idLength)" << std::endl;
            return false;
        }
        if( !is.good() ) {
            common::error() << "image::load_tga() error: reading error (header.idLength !is.good())" << std::endl;
            is.seekg( pos );
            return false;
        }
    }
    
    pixel_format tgaFmt = PIXEL_FORMAT_AUTO;
    switch( header.bpp ) {
        case 8:
            tgaFmt = PIXEL_FORMAT_GRAY8;
            break;
        case 16:
            tgaFmt = PIXEL_FORMAT_BGRA8;
            break;
        case 24:
            tgaFmt = PIXEL_FORMAT_BGR8;
            break;
        case 32:
            tgaFmt = PIXEL_FORMAT_BGRA8;
            break;
        default:
            assert(0);
    }
    assert( tgaFmt != PIXEL_FORMAT_AUTO );
    fmt = fmt == PIXEL_FORMAT_AUTO ? tgaFmt : fmt;
    int tgaPxSize = header.bpp >> 3;
    fnPxcvtFunc cvt = get_pxcvt_func( tgaFmt, fmt );
    
    reserve( header.width, header.height, fmt );

    if( header.dataType == 2 || header.dataType == 3 ) { 
        /* Uncompressed data */
        for( int i = 0; i < header.height; i++ ) {
            byte *data = this->data.data() + this->stride * (height - i - 1);
            for( int j = 0; j < header.width; j++ ) {
                /* Read one pixel data from file */
                if( !is.read( reinterpret_cast<char*>(buffer), tgaPxSize ) ) {
                    release();
                    common::error() << "image::load_tga() error: reading error (read data)" << std::endl;
                    is.seekg( pos );
                    return false;
                }
                if( header.bpp == 16 ) {
                    /* firstly convert to supported pixel format */
                    tga_16_to_bgra8( buffer );
                }
                cvt( buffer, data );
                data += this->bpp >> 3;
            }
        }
    } else if ( header.dataType == 10 || header.dataType == 11 ) {
        if( !load_tga_read_rle( is, header, this->data.data(), this->bpp >> 3, cvt, this->stride ) ) {
            release();
            is.seekg( pos );
            return false;
        }
    }

    /* flip image */
    if( header.attrib & 0x20 ) {
        vertical_flip();
    }
    if( header.attrib & 0x10 ) {
        horizontal_flip();
    }

    return true;
}

/* tga_get_best_save_format */
static pixel_format tga_get_best_save_format( pixel_format fmt ) {
    switch( fmt ) {
        case PIXEL_FORMAT_GRAY8:
            return PIXEL_FORMAT_GRAY8;
        case PIXEL_FORMAT_RGB8:
        case PIXEL_FORMAT_BGR8:
            return PIXEL_FORMAT_BGR8;
        case PIXEL_FORMAT_RGBA8:
        case PIXEL_FORMAT_BGRA8:
            return PIXEL_FORMAT_BGRA8;
        case PIXEL_FORMAT_AUTO:
            break;
    }
    assert(0);
    return PIXEL_FORMAT_AUTO;
}

/* image::save_tga */
bool image::save_tga( ostream &os, pixel_format fmt, bool rle, byte compress ) {
    targa_header header;
    byte buffer[256 * 4];
    
    /* if needed then get best pixel format to save */
    if( fmt == PIXEL_FORMAT_AUTO ) {
        fmt = tga_get_best_save_format( this->fmt );
    }
    auto tgaBpp = pixel_format_to_bpp( fmt );
    auto tgaPxSize = tgaBpp >> 3;
    auto pxSize = get_bpp() >> 3;
    auto cvt = get_pxcvt_func( this->fmt, fmt );

    /* fill tga header */
    header.idLength = 0;
    header.colormapType = 0;
    header.dataType = tgaBpp == 8 ? (rle ? 11 : 3) : (rle ? 10 : 2);
    header.colormapIndex = 0;
    header.colormapLength = 0;
    header.colormapSize = 0;
    header.xOrigin = 0;
    header.yOrigin = 0;
    header.width = static_cast<dword>(this->width);
    header.height = static_cast<dword>(this->height);
    header.bpp = tgaBpp;
    header.attrib = 0;

    /* Write targa header */
    if( !os.write( reinterpret_cast<char*>(&header), sizeof(header) ) ) {
        common::error() << "image::save_tga() error: writing error (header)" << std::endl;
        return false;
    }

    /* Save tga image */
    if( rle ) {
        auto cmp = get_pxcmp_func( this->fmt );
        auto pxCount = this->width * this->height;
        auto get_linear_pixel_ptr = [this]( int i ) {
            int line = i / this->width;
            int y = this->height - line - 1;
            int x = i - line * this->width;
            return this->get_pixel_ptr( x, y );
        };
        for( int i = 0; i < pxCount; ) {
            int k = 1;
            auto firstPx = get_linear_pixel_ptr( i );
            cvt( firstPx, buffer + 1 );
            for( ; (i + k < pxCount) && (k < 128); k++ ) {
                auto px = get_linear_pixel_ptr( i + k );
                if( core::math::abs( cmp( firstPx, px ) ) > compress ) {
                    break;
                }
            }
            if( k > 1 ) {
                /* write rle packet */
                buffer[0] = 0x80 | static_cast<byte>((k - 1) & 0x7f);
                i += k;
                if( !os.write( reinterpret_cast<char*>(buffer), 1 + tgaPxSize ) ) {
                    common::error() << "image::save_tga() error: writing error (rle packet)" << std::endl;
                    return false;
                }
                continue;
            }
            auto prevPx = firstPx;
            for( ; (i + k < pxCount) && (k < 128); k++ ) {
                auto px = get_linear_pixel_ptr( i + k );
                if( core::math::abs( cmp( prevPx, px ) ) <= compress ) {
                    break;
                }
                /* copy pixel */
                cvt( px, buffer + k * tgaPxSize + 1 );
                prevPx = px;
            }
            buffer[0] = static_cast<byte>((k - 1) & 0x7f);
            if( !os.write( reinterpret_cast<char*>(buffer), 1 + tgaPxSize * k ) ) {
                common::error() << "image::save_tga() error: writing error (rle packet)" << std::endl;
                return false;
            }
            i += k;
        }
    } else {
        /* Write uncompressed targa data */
        for( int i = 0; i < this->height; i++ ) {
            byte *data = this->data.data() + this->stride * (this->height - i - 1);
            for( int j = 0; j < this->width; j++ ) {
                cvt( data, buffer );
                if( !os.write( reinterpret_cast<char*>(buffer), tgaPxSize ) ) {
                    common::error() << "image::save_tga() error: writing error (data)" << std::endl;
                    return false;
                }
                data += pxSize;
            }
        }
    }
    return true;
}

/*
================================================
            Load / write JPEG files
================================================
*/

/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * Our example here shows how to override the "error_exit" method so that
 * control is returned to the library's caller when a fatal error occurs,
 * rather than calling exit() as the standard error_exit method does.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

void my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = reinterpret_cast<my_error_ptr>(cinfo->err);

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);
  //common::error() << "JPEG error: " << (*cinfo->err->output_message) (cinfo) << std::endl;

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/* my_jreader
*/
static int my_jreader( void *file, void *buf, int size ) {
    assert(file != nullptr);
    istream *is = reinterpret_cast<istream*>(file);
    is->read( reinterpret_cast<char*>(buf), size );
    auto nbytes = is->gcount();
    return static_cast<int>(nbytes);
}

/* image::load_jpg */
bool image::load_jpg( istream &is, pixel_format fmt ) {
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  auto pos = is.tellg();
  /* get file size */
 

  int row_stride;       /* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    common::error() << "image::load_jpg() error: loading jpeg file" << std::endl;
    is.seekg( pos, is.beg );
    return false;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */
    jpeg_datarw_struct jreader;
    jreader.read = my_jreader;
    jreader.file = reinterpret_cast<void*>(&is);
  jpeg_reader_src( &cinfo, &jreader );


  /* Step 3: read file parameters with jpeg_read_header() */
  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */
  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */
  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  auto jpegFmt = PIXEL_FORMAT_AUTO;
  row_stride = cinfo.output_width * cinfo.output_components;

    switch( cinfo.output_components ) {
        case 1:
            jpegFmt = PIXEL_FORMAT_GRAY8;
            break;
        case 3:
            jpegFmt = PIXEL_FORMAT_RGB8;
            break;
        case 4:
            jpegFmt = PIXEL_FORMAT_RGBA8;
            break;
        default:
            common::error() << "image::load_jpg() error: unsupported JPEG components '" 
                    << cinfo.output_components << "'" << std::endl;
            is.seekg( pos, is.beg );
            return false;
    }

    if( fmt == PIXEL_FORMAT_AUTO ) {
        fmt = jpegFmt;
    }

  /* Make a one-row-high sample array that will go away when done with image */
    reserve( cinfo.output_width, cinfo.output_height, fmt );


  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */
  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
    byte *dataPtr = this->data.data();
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    /* Assume put_scanline_someplace wants a pointer and sample count. */

    auto bufLine = dataPtr + (row_stride * cinfo.output_scanline);
    (void) jpeg_read_scanlines(&cinfo, &bufLine, 1);
  }

  /* Step 7: Finish decompression */
  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);
  
  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
    return true;
}


/* my_jwriter
*/
static int my_jwriter( void *file, const void *buf, int size ) {
    assert(file != nullptr);
    ostream *os = reinterpret_cast<ostream*>(file);
    auto before = os->tellp();
    os->write( reinterpret_cast<const char*>(buf), size );
    auto nbytes = os->tellp() - before;
    return static_cast<int>(nbytes);
}

/* image::save_jpg */
bool image::save_jpg( ostream &os, pixel_format fmt, int quality ) {
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */

    jpeg_datarw_struct writer;
    writer.write = my_jwriter;
    writer.file = reinterpret_cast<void*>(&os);
  jpeg_writer_dest(&cinfo, &writer );

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = this->width;  /* image width and height, in pixels */
  cinfo.image_height = this->height;

  if( fmt == PIXEL_FORMAT_AUTO ) {
    switch( this->fmt ) {
        case PIXEL_FORMAT_AUTO:
            assert(0);
            break;
        case PIXEL_FORMAT_GRAY8:
            cinfo.input_components = 1;
            cinfo.in_color_space = JCS_GRAYSCALE;
            break;
        case PIXEL_FORMAT_BGR8:
        case PIXEL_FORMAT_BGRA8:
        case PIXEL_FORMAT_RGB8:
        case PIXEL_FORMAT_RGBA8:
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
            fmt = PIXEL_FORMAT_RGB8;
            break;
    }
  } else {
    switch( fmt ) {
        case PIXEL_FORMAT_AUTO:
            assert(0);
            break;
        case PIXEL_FORMAT_GRAY8:
            cinfo.input_components = 1;
            cinfo.in_color_space = JCS_GRAYSCALE;
            break;
        case PIXEL_FORMAT_BGR8:
        case PIXEL_FORMAT_BGRA8:
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
            fmt = PIXEL_FORMAT_BGR8;
            break;
        case PIXEL_FORMAT_RGB8:
        case PIXEL_FORMAT_RGBA8:
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
            fmt = PIXEL_FORMAT_RGB8;
            break;
    }
  }
  //cinfo.input_components = 3;       /* # of color components per pixel */
  //cinfo.in_color_space = JCS_RGB;   /* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality( &cinfo, quality, TRUE /* limit to baseline-JPEG values */ );

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  auto cvt = get_pxcvt_func( this->fmt, fmt );
  auto jpgPxSize = pixel_format_to_bpp( fmt ) >> 3;
  auto srcPxSize = get_bpp() >> 3;
  core::vector<byte> bufferCvtData;
  bufferCvtData.reserve( jpgPxSize * this->width );
  byte *jpgDataPtr = bufferCvtData.data();

  while( cinfo.next_scanline < cinfo.image_height ) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    /* convert to new pixel format */
    byte *jpgPx = jpgDataPtr;
    byte *srcPx = get_line_ptr( cinfo.next_scanline );
    for( int i = 0; i < this->width; i++ ) {
        cvt( srcPx, jpgPx );
        srcPx += srcPxSize;
        jpgPx += jpgPxSize;
    }

    (void) jpeg_write_scanlines(&cinfo, &jpgDataPtr, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */

    return true;
}



/*
================================================
            Loading/writing PNG files
================================================
*/

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif

static void png_error_callback( png_structp png, png_const_charp cstr ) {
    common::error() << "image::load_png() error: png_error_callback(): " << cstr << std::endl;
    longjmp( png_jmpbuf(png), 1 );
}

static void png_warning_callback( png_structp png, png_const_charp cstr ) {
    common::log() << "call: png_warning_callback()" << std::endl;
    (void)png;
    (void)cstr;
}

static void png_progressive_info_callback( png_structp png, png_infop info ) {
    common::log() << "call: png_progressive_info_callback()" << std::endl;
    (void)png;
    (void)info;
}

static void png_progressive_row_callback( png_structp png, png_bytep data, png_uint_32 row, int size ) {
    common::log() << "call: png_progressive_row_callback()" << std::endl;
    (void)png;
    (void)data;
    (void)row;
    (void)size;
}

static void png_progressive_end_callback( png_structp png, png_infop info ) {
    (void)png;
    (void)info;
    common::log() << "call: png_progressive_end_callback()" << std::endl;
}

static void png_read_data_callback( png_structp png, png_bytep data, size_t length ) {
   /* fread() returns 0 on error, so it is OK to store this in a size_t
    * instead of an int, which is what fread() actually returns.
    */
    istream *is( reinterpret_cast<istream*>(png_get_io_ptr(png)) );
    assert( is != nullptr );

    is->read( reinterpret_cast<char*>(data), length );

    size_t nbytes = static_cast<size_t>( is->gcount() );

    if( nbytes != length ) {
        png_error( png, "PNG Reading Error. nbytes != length" );
    }
}

static bool png_check_signature( istream &is ) {
    char buf[4];
    auto pos = is.tellg();
    if( !is.read( buf, sizeof(buf) ) ) {
        is.seekg( pos );
        return false;
    }
    is.seekg( pos );
    return( !png_sig_cmp( reinterpret_cast<png_const_bytep>(buf), 0, 4 ) );
}

/* image::load_png */
bool image::load_png( istream &is, pixel_format fmt ) {
    if( !png_check_signature(is) ) {
        return false;
    }
   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible, in case we are using dynamically
    * linked libraries.
    */
    png_structp png = png_create_read_struct( PNG_LIBPNG_VER_STRING,
        nullptr, png_error_callback, png_warning_callback );
    if( png == nullptr ) {
        common::error() << "image::load_png() error: png_create_read_struct() returns nullptr" << std::endl;
        return false;
    }
    png_infop info = png_create_info_struct( png );
    if( info == nullptr ) {
        png_destroy_read_struct( &png, &info, NULL );
        common::error() << "image::load_png() error: png_create_info_struct() returns nullptr" << std::endl;
        return false;
    }
    if( setjmp( png_jmpbuf( png ) ) ) {
        png_destroy_read_struct( &png, &info, NULL );
        common::error() << "image::load_png() error: setjmp() worked" << std::endl;
        return false;
    }

   /* You will need to provide all three function callbacks,
    * even if you aren't using all of them.
    * If you aren't using all functions, you can specify NULL
    * parameters.  Even when all three functions are NULL,
    * you need to call png_set_progressive_read_fn().
    * These functions shouldn't be dependent on global or
    * static variables if you are decoding several images
    * simultaneously.  You should store stream specific data
    * in a separate struct, given as the second parameter,
    * and retrieve the pointer from inside the callbacks using
    * the function png_get_progressive_ptr(png_ptr).
    */
    //png_set_read_fn( png, reinterpret_cast<png_voidp>(&is), png_read_data_callback );

    png_set_progressive_read_fn( png, reinterpret_cast<png_voidp>(&is),
        png_progressive_info_callback, png_progressive_row_callback, 
        png_progressive_end_callback );
    png_set_read_fn( png, reinterpret_cast<png_voidp>(&is), png_read_data_callback );
   // png_set_sig_bytes( png, 6 );
   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED.
    */

    png_read_info( png, info );


    png_uint_32 width, height;
    int bitDepth, colorType, interlaceType;
    if( !png_get_IHDR( png, info, &width, &height, &bitDepth, &colorType,
        &interlaceType, NULL, NULL ) ) {
        common::error() << "image::load_png() error: png_get_IHDR() failure" << std::endl;
        png_destroy_read_struct( &png, &info, NULL );
        return false;
    }

    /*common::log() << "png width: " << width << std::endl;
    common::log() << "png height: " << height << std::endl;
    common::log() << "png bitDepth: " << bitDepth << std::endl;
    common::log() << "png colorType: " << colorType << std::endl;
    common::log() << "png interlaceType: " << interlaceType << std::endl;
    */

   /* Set up the data transformations you want.  Note that these are all
    * optional.  Only call them if you want/need them.  Many of the
    * transformations only work on specific types of images, and many
    * are mutually exclusive.
    */

    pixel_format pngFmt = PIXEL_FORMAT_AUTO;
    switch( colorType ) {
        case PNG_COLOR_TYPE_GRAY:
            pngFmt = PIXEL_FORMAT_GRAY8;
            break;
        case PNG_COLOR_TYPE_RGB:
            pngFmt = PIXEL_FORMAT_RGB8;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            pngFmt = PIXEL_FORMAT_RGBA8;
            break;
        default:
            common::error() << "image::load_png() error: unsupported PNG pixel format" << std::endl;
            png_destroy_read_struct( &png, &info, NULL );
            return false;
    }
    if( fmt == PIXEL_FORMAT_AUTO ) {
        fmt = pngFmt;
    }

    reserve( width, height, fmt );
    
    int pngPxSize = pixel_format_to_bpp( pngFmt ) >> 3;
    int pxSize = get_bpp() >> 3;
    core::vector<byte> tempData;
    tempData.reserve( pngPxSize * width );
    byte *pngDataPtr = tempData.data();
    auto cvt = get_pxcvt_func( pngFmt, fmt );
    for( int y = 0; y < this->height; y++ ) {
        png_read_rows( png, reinterpret_cast<png_bytepp>(&pngDataPtr), NULL, 1 );
        byte *from = pngDataPtr;
        byte *to = get_line_ptr( y );
        for( int i = 0; i < this->width; i++ ) {
            cvt( from, to );
            from += pngPxSize;
            to += pxSize;
        }
    }
   
    png_destroy_read_struct( &png, &info, NULL );
    return true;
}

} /* namespace renderer */
} /* namespace engine */