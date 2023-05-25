#ifndef SCENE_HPP
#define SCENE_HPP

#include "icosahedron.hpp"
#include "shader_program.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>

extern "C" {
#include <png.h>
}
#include <filesystem>

class Scene {
  GLFWwindow *window;
protected:
  int WIDTH, HEIGHT;
public:
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mode);

  Scene(std::string const &windowTitle, int width, int height);
  virtual ~Scene();
  void SaveImage(std::string const &directory, std::string const &name);

  void RenderLoop(GLfloat red = 0.f, GLfloat green = 0.f,
                       GLfloat blue = 0.f, GLfloat alpha = 1.f);

  // Load models, textures, initialize shaders, etc.
  virtual void InitScene() {}

  // Draw your scene
  virtual void Render() {}
};
#endif // SCENE_HPP