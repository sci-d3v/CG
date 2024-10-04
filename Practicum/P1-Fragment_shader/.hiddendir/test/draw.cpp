#include <png.h>
#include <stdio.h>
#include <GL/glut.h>

void
create_png()
{
   char *filename;
   asprintf(&filename, PIC_FILE_NAME_FORMAT, WORK_DIRECTORY, PIC_DIRECTORY, collection_index,
            iteration_index);
   printf("%s\n", filename);
   FILE *fp = fopen(filename, "wb");

   if(!fp)
   {
      printf(TXT_ERR_FILE_CREATING, filename);
      exit(1);
   }

   png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if(!png_ptr)
   {
      printf(TXT_ERR_CREATE_OBJECT);
      fclose(fp);
      exit(1);
   }

   png_infop png_info;
   if(!(png_info = png_create_info_struct(png_ptr)) || setjmp(png_jmpbuf(png_ptr)))
   {
      printf(TXT_ERR_CREATE_OBJECT);
      png_destroy_write_struct(&png_ptr, NULL);
      fclose(fp);
      exit(1);
   }

   png_init_io(png_ptr, fp);
   png_set_IHDR(png_ptr, png_info, PIC_WIDTH, PIC_HEIGHT, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);

   png_set_compression_level(png_ptr, 9);

   GLubyte *pixels = malloc(4 * PIC_WIDTH * PIC_HEIGHT);   // rgba = 4 bytes
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glReadPixels(0, 0, PIC_WIDTH, PIC_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
   unsigned char *rows[PIC_HEIGHT];

   for(int i = 0; i < PIC_HEIGHT; ++i)
   {
      rows[PIC_HEIGHT - i - 1] = pixels + (i * PIC_WIDTH * 4);
   }
   png_set_rows(png_ptr, png_info, rows);
   png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
   png_write_end(png_ptr, png_info);

#if defined(DEBUG)
   printf("screenshoot has been done!\n");
#endif

   png_destroy_write_struct(&png_ptr, NULL);
   fclose(fp);
}


#include <vector>

void saveImage(char *filepath, GLFWwindow *w) {
  int width, height;
  glfwGetFramebufferSize(w, &width, &height);
  GLsizei nrChannels = 3;
  GLsizei stride = nrChannels * width;
  stride += (stride % 4) ? (4 - stride % 4) : 0;
  GLsizei bufferSize = stride * height;
  std::vector<char> buffer(bufferSize);
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glReadBuffer(GL_FRONT);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
  stbi_flip_vertically_on_write(true);
  stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}
