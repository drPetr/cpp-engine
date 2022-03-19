#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "jdatarw.h"

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */
  struct jpeg_datarw_struct *rw; /* reader/writer */
  JOCTET * buffer;      /* start of buffer */
} my_destination_mgr_w;

typedef my_destination_mgr_w * my_dest_ptr;

#define OUTPUT_BUF_SIZE  1024*32   /* choose an efficiently fwrite'able size */

METHODDEF(void)
init_destination_w (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


METHODDEF(boolean)
empty_output_buffer_w (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;


  if (dest->rw->write(dest->rw->file, dest->buffer, OUTPUT_BUF_SIZE) !=
      (size_t) OUTPUT_BUF_SIZE)
    ERREXIT(cinfo, JERR_FILE_WRITE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}


METHODDEF(void)
term_destination_w (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  /* Write any data remaining in the buffer */
  if (datacount > 0) {
    if (dest->rw->write(dest->rw->file, dest->buffer, datacount) != datacount)
      ERREXIT(cinfo, JERR_FILE_WRITE);
  }
  //fflush(dest->outfile);
  /* Make sure we wrote the output file OK */
  //if (ferror(dest->outfile))
  //  ERREXIT(cinfo, JERR_FILE_WRITE);
}

GLOBAL(void)
jpeg_writer_dest (j_compress_ptr cinfo, struct jpeg_datarw_struct *rw )
{
  my_dest_ptr dest;

  if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                  SIZEOF(my_destination_mgr_w));
  }

  dest = (my_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination_w;
  dest->pub.empty_output_buffer = empty_output_buffer_w;
  dest->pub.term_destination = term_destination_w;
  dest->rw = rw;
}
