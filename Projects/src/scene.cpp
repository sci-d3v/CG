#include "scene.hpp"
#include "shader_program.hpp"
#include <filesystem>
#include <memory>
#include <string>

Scene::Scene(std::string const &windowTitle, int width, int height)
    : WIDTH(width), HEIGHT(height) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  // Init GLFW
  if (!glfwInit())
    exit(EXIT_FAILURE);
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFW window object that we can use for GLFW's functions
  window =
      glfwCreateWindow(WIDTH, HEIGHT, windowTitle.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, KeyCallback);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to set up the OpenGL Function pointers
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);
}

Scene::~Scene() {
  glfwTerminate();
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void Scene::RenderLoop(GLfloat red, GLfloat green ,
                       GLfloat blue, GLfloat alpha ) {
  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  InitScene();
  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // render
    // ------
    glClearColor(red,green,blue,alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Render();
    glfwSwapBuffers(window);
    // Poll IO events (keys pressed/released, mouse moved etc.)
    glfwPollEvents();
  }
}

void Scene::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mode) {
  // Is called whenever a key is pressed/released via GLFW
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  // glfwSetWindowSize(window, WIDTH, HEIGHT);
}

char const *PIC_FILE_NAME_FORMAT = ".png";
char const *TXT_ERR_FILE_CREATING = "TXT_ERR_FILE_CREATING";
char const *TXT_ERR_CREATE_OBJECT = "TXT_ERR_CREATE_OBJECT";

/*
  How to make an gif animation from png files in linux
  convert -delay 30 -loop 0 *.png animation.gif
  It works slowly and I've been searching better solution.
*/
void Scene::SaveImage(std::string const &directory, std::string const &name) {

  std::filesystem::path imagePath(directory);
  imagePath.append(name + PIC_FILE_NAME_FORMAT);
  imagePath = std::filesystem::absolute(imagePath);

  // std::cout<<imagePath<<std::endl;
  // std::ofstream file(imagePath, std::ios::binary);
  // open files
  // file.open(path.c_str());
  // std::stringstream shaderStream;
  // // read file's buffer contents into streams
  // shaderStream << file.rdbuf();

  // std::ofstream file(imagePath, std::ios::binary);
  // if (!file) {
  //   std::cout << "Error creating file: " << imagePath << std::endl;
  //   return;
  // }

  // std::filebuf *fileBuf = file.rdbuf();
  // FILE *fp = fileBuf->;

  FILE *fp = fopen(imagePath.c_str(), "wb");

  if (!fp) {
    std::cout << imagePath << std::endl;
    std::cout << TXT_ERR_FILE_CREATING << std::endl;
    exit(1);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    std::cout << TXT_ERR_CREATE_OBJECT << std::endl;
    fclose(fp);
    exit(1);
  }

  png_infop png_info;
  if (!(png_info = png_create_info_struct(png_ptr)) ||
      setjmp(png_jmpbuf(png_ptr))) {
    std::cout << TXT_ERR_CREATE_OBJECT << std::endl;
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    exit(1);
  }

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, png_info, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_DEFAULT);

  png_set_compression_level(png_ptr, 9);

  // rgba = 4 bytes
  std::unique_ptr<GLubyte[]> pixels(new GLubyte[4 * WIDTH * HEIGHT]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

  unsigned char *rows[HEIGHT];
  for (size_t i = 0; i < HEIGHT; ++i) {
    rows[HEIGHT - i - 1] = pixels.get() + (i * WIDTH * 4);
  }
  png_set_rows(png_ptr, png_info, rows);
  png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
  png_write_end(png_ptr, png_info);

  png_destroy_write_struct(&png_ptr, NULL);
  fclose(fp);
}

/*
void saveOpenGLImageAsPNG(const std::string& imagePath, int width, int height) {
    std::ofstream file(imagePath, std::ios::binary);
    if (!file) {
        std::cout << "Error creating file: " << imagePath << std::endl;
        return;
    }

    std::filebuf* fileBuf = file.rdbuf();
    FILE* fp = fileBuf->file();

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
nullptr, nullptr, nullptr); if (!png_ptr) { std::cout << "Error creating PNG
write structure." << std::endl; return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cout << "Error creating PNG info structure." << std::endl;
        png_destroy_write_struct(&png_ptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "Error during PNG writing." << std::endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
PNG_FILTER_TYPE_DEFAULT); png_write_info(png_ptr, info_ptr);

    std::vector<uint8_t> pixelData(4 * width * height);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
pixelData.data());

    std::vector<png_bytep> rows(height);
    for (int i = 0; i < height; ++i) {
        rows[i] = reinterpret_cast<png_bytep>(pixelData.data() + (i * width *
4));
    }

    png_write_image(png_ptr, rows.data());
    png_write_end(png_ptr, nullptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    file.close();

    std::cout << "Image saved successfully: " << imagePath << std::endl;
}
*/