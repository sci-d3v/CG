#ifndef MS_SCENE_HPP
#define MS_SCENE_HPP

#include "scene.hpp"
#include "shader.hpp"
#include "shader_program.hpp"
#include <cmath>
#include <glm/fwd.hpp>
#include <string>

class MandelbrotSetScene : public Scene {
  std::unique_ptr<Make_ShaderProgram<2>::type> shaderProgram;
  unsigned int VAO, VBO;
  std::array<GLfloat, 8> data = {-1.0f, -1.0f, 1.0f, -1.0f,
                                 -1.0f, 1.0f,  1.0f, 1.0f};

public:
  MandelbrotSetScene(std::string const &windowTitle, int width, int height)
      : Scene(windowTitle, width, height) {
#ifndef NDEBUG
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
  }

  void InitScene() override {
    shaderProgram = std::make_unique<Make_ShaderProgram<2>::type>(
        Shader("shaders/vertex.glsl", GL_VERTEX_SHADER),
        Shader("shaders/ms_fragment.glsl", GL_FRAGMENT_SHADER));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s)
    // and attribute pointer(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                          (GLvoid *)0);
    glEnableVertexAttribArray(0);
    // Unbind starVAO
    glBindVertexArray(0);
  }

  int fractal_iteration = 1;
  glm::vec2 scale = {3, 3};

  void Render() override {
    // draw points
    shaderProgram->Use();
    shaderProgram->SetUniform("u_resolution", glUniform2f, this->WIDTH,
                              this->HEIGHT);
    shaderProgram->SetUniform("u_fractal_iteration", glUniform1i,
                              fractal_iteration * 2);
    fractal_iteration++;
    shaderProgram->SetUniform("u_shift", glUniform2f,
                              1., // 1.401155,
                              0.0);
    shaderProgram->SetUniform("u_scale", glUniform2f, scale.x, scale.y);
    // scale *= 0.99;

    // Draw 
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, data.size() / 2);
    glBindVertexArray(0);
    if(fractal_iteration<36)
    SaveImage("images", "picture_" + std::to_string(fractal_iteration));
  }

  ~MandelbrotSetScene() {
#ifndef NDEBUG
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }
};

#endif // MS_SCENE_HPP