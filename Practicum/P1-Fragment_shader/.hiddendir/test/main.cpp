//https://www.countryflags.com/
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void create_png();

const GLuint WIDTH = 1000, HEIGHT = 1000;
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode);

GLchar *glShaderLoad(char *path);

int main(int argc, char *argv[]) {
  // Init GLFW
  glfwInit();
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "CG_P1_Test", NULL, NULL);
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  glewInit();

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  // Build and compile our shader program
  // Vertex shader
  const GLchar *vertexShaderSource = glShaderLoad("shaders/vertex.glsl");
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // Check for compile time errors
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
  }

  // Fragment shader
  const GLchar *fragmentShaderSource = glShaderLoad("shaders/fragment.glsl");
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // Check for compile time errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
  }

  // Link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // Check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLfloat constellation[] = {-1.0f, -1.0f, 1.0f, -1.0f,
                             -1.0f, 1.0f,  1.0f, 1.0f};
  GLuint constellationVBO, constellationVAO;
  glGenVertexArrays(1, &constellationVAO);
  glGenBuffers(1, &constellationVBO);
  // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and
  // attribute pointer(s).
  glBindVertexArray(constellationVAO);

  glBindBuffer(GL_ARRAY_BUFFER, constellationVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(constellation), constellation,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0); // Unbind starVAO

  // Game loop
  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    // Render
    // Clear the color buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    GLint u_resolutionLocation =
        glGetUniformLocation(shaderProgram, "u_resolution");
    glUniform2f(u_resolutionLocation, WIDTH, HEIGHT);

    // Draw the constellation
    glBindVertexArray(constellationVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    // Swap the screen buffers
    glfwSwapBuffers(window);
  }
  create_png();
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &constellationVAO);
  glDeleteBuffers(1, &constellationVBO);
  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();
  return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  glfwSetWindowSize(window, 600, 600);
}

// Параметры, с которыми производились расчеты, лежат в заголовочном файле
// нужно возвращать ошибку!
GLchar *glShaderLoad(char *path) {
  GLchar *shaderSource = NULL;

  int descriptor = open(path, O_RDONLY);

  if (descriptor != -1) {
    FILE *file = fdopen(descriptor, "rb");

    if (file) {
      struct stat statistics;

      if (fstat(descriptor, &statistics) != -1) {
        shaderSource = (GLchar *)malloc(statistics.st_size);
        fread(shaderSource, sizeof(char), statistics.st_size, file);
      }
      fclose(file);
    }
    close(descriptor);
  }
  printf("%s\n", path);
  printf("%s\n", shaderSource);
  return shaderSource;
}

#include <png.h>
#include <stdio.h>
// #include <GL/glut.h>

// #define PIC_FILE_NAME_FORMAT ".png"
// #define WORK_DIRECTORY ""
// #define PIC_DIRECTORY ""
#define TXT_ERR_FILE_CREATING "TXT_ERR_FILE_CREATING"
#define TXT_ERR_CREATE_OBJECT "TXT_ERR_CREATE_OBJECT"
#define PIC_WIDTH WIDTH
#define PIC_HEIGHT HEIGHT

void create_png() {
  char *filename = "./test.png";
  //  asprintf(&filename, PIC_FILE_NAME_FORMAT, WORK_DIRECTORY, PIC_DIRECTORY);
  //  printf("%s\n", filename);

  FILE *fp = fopen(filename, "wb");

  if (!fp) {
    printf(TXT_ERR_FILE_CREATING, filename);
    exit(1);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    printf(TXT_ERR_CREATE_OBJECT);
    fclose(fp);
    exit(1);
  }

  png_infop png_info;
  if (!(png_info = png_create_info_struct(png_ptr)) ||
      setjmp(png_jmpbuf(png_ptr))) {
    printf(TXT_ERR_CREATE_OBJECT);
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    exit(1);
  }

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, png_info, PIC_WIDTH, PIC_HEIGHT, 8,
               PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);

  png_set_compression_level(png_ptr, 9);

  GLubyte *pixels = new GLubyte[4 * PIC_WIDTH * PIC_HEIGHT]; // rgba = 4 bytes
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, PIC_WIDTH, PIC_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  unsigned char *rows[PIC_HEIGHT];

  for (int i = 0; i < PIC_HEIGHT; ++i) {
    rows[PIC_HEIGHT - i - 1] = pixels + (i * PIC_WIDTH * 4);
  }
  png_set_rows(png_ptr, png_info, rows);
  png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
  png_write_end(png_ptr, png_info);

  png_destroy_write_struct(&png_ptr, NULL);
  fclose(fp);
  delete[] pixels;
}