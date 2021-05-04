#include "stdio.h"
#include "setjmp.h"
#include "jpeglib.h"
#include "stdlib.h"

int main()
{

  struct jpeg_decompress_struct dinfo;

  // 声明错误处理器
  struct jpeg_error_mgr jem;
  dinfo.err = jpeg_std_error(&jem);

  // Init jpeg decoder
  jpeg_create_decompress(&dinfo);

  // hardcode the file
  char *filename = "star.jpeg";

  FILE *picture;

  if ((picture = fopen(filename, "rb")) == NULL)
  {
    fprintf(stderr, "can't open %s\n", filename);
    return -1;
  }

  printf("Open file successfully");

  // Read throught file location
  jpeg_stdio_src(&dinfo, picture);

  // Read file config info
  jpeg_read_header(&dinfo, TRUE);

  // decompress jpeg
  jpeg_start_decompress(&dinfo);

  // Get buffer of file
  unsigned char *buffer = (unsigned char *)malloc(dinfo.output_width * dinfo.output_components);

  // Get origin picture array
  unsigned char *origin = (unsigned char *)malloc(dinfo.output_width * dinfo.output_components * dinfo.output_height);

  printf("width: %d, height: %d, comp: %d", dinfo.output_width, dinfo.output_height, dinfo.output_components);
  int line = 0;
  while (dinfo.output_scanline < dinfo.output_height)
  {
    jpeg_read_scanlines(&dinfo, &buffer, 1);
    for (int i = 0; i < dinfo.output_width * 3; i++)
    {
      // printf("%02X%02X%02X ", buffer[i * 3], buffer[i * 3 + 1], buffer[i * 3 + 2]);
      origin[line * dinfo.output_width * 3 + i] = buffer[i];
    }
    line++;
  }

  jpeg_finish_decompress(&dinfo);
  jpeg_destroy_decompress(&dinfo);

  fclose(picture);


  


  /**
   * Writting file
   */

  struct jpeg_compress_struct cinfo;

  struct jpeg_error_mgr crr;

  cinfo.err = jpeg_std_error(&crr);

  char *destFile = "out.jpeg";
  FILE *dest;

  if ((dest = fopen(destFile, "wb")) == NULL)
  {
    fprintf(stderr, "can't open %s\n", destFile);
    return -1;
  }

  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, dest);

  cinfo.image_width = 1280;
  cinfo.image_height = 800;

  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 80, TRUE);

  // compress
  jpeg_start_compress(&cinfo, 1);

  JSAMPROW row_pointer[1];

  int row_stride; // 每一行的字节数

  row_stride = cinfo.image_width * 3; // 若是不是索引图,此处须要乘以3

  int temp = 0;

  while (cinfo.next_scanline < cinfo.image_height)
  {
    char *color = malloc(cinfo.image_width * 3);
    for (int i = 0; i < cinfo.image_width * 3; i++)
    {
      color[i] = origin[temp * cinfo.image_width * 3 + i];
    }
    row_pointer[0] = color;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
    temp++;
    free(color);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  free(origin);

  return 0;
}
