#ifndef __JDATARW_H__
#define __JDATARW_H__

#include "jpeglib.h"

/*                      file, buffer, size*/
typedef int(*jfn_jpeg_reader)(void*, void*, int);
typedef int(*jfn_jpeg_writer)(void*, const void*, int);

struct jpeg_datarw_struct{
    void                *file;
    jfn_jpeg_reader     read;
    jfn_jpeg_writer     write;
};

EXTERN(void) jpeg_writer_dest JPP((j_compress_ptr cinfo, struct jpeg_datarw_struct *rw));
EXTERN(void) jpeg_reader_src JPP((j_decompress_ptr cinfo, struct jpeg_datarw_struct *rw));

#endif /* __JDATARW_H__ */