#ifndef SCENE_HPP
#define SCENE_HPP

#include "shader_program_impl.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>

#include <filesystem>

#include "shader_program_impl.hpp"
#include <cmath>

#include <map>

class Application {
protected:
  GLFWwindow *window;
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mode);
  static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);

  std::unique_ptr<Make_ShaderProgram<1>::type> computeSP;
  std::unique_ptr<Make_ShaderProgram<2>::type> renderSP;
  int WIDTH, HEIGHT;

  unsigned int imageTexture;
  unsigned int quadVAO;
  unsigned int quadVBO;

public:
  ~Application();
  Application(std::string const &windowTitle, int width, int height);

  void InitApplication();
  void InitQuad();
  void InitTextures();
  
  void RenderLoop();
  void Render();
};

#endif // SCENE_HPP
