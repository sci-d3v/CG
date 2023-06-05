#ifndef JS_SCENE_HPP
#define JS_SCENE_HPP

#include "scene.hpp"
#include "shader.hpp"
#include "shader_program.hpp"
#include <cmath>
#include <glm/fwd.hpp>
#include <string>

class JuliaSetScene : public Scene {
  std::unique_ptr<Make_ShaderProgram<2>::type> shaderProgram;
  unsigned int VAO, VBO;
  std::array<GLfloat, 8> data = {-1.0f, -1.0f, 1.0f, -1.0f,
                                 -1.0f, 1.0f,  1.0f, 1.0f};
  int const pic_count = 36;
  int pic_iteration = 0;

public:
  JuliaSetScene(std::string const &windowTitle, int width, int height)
      : Scene(windowTitle, width, height) {
#ifndef NDEBUG
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
  }

  void InitScene() override {
    shaderProgram = std::make_unique<Make_ShaderProgram<2>::type>(
        Shader("shaders/vertex.glsl", GL_VERTEX_SHADER),
        Shader("shaders/js_fragment.glsl", GL_FRAGMENT_SHADER));

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

  int fractal_iteration = 25 * 1;
  glm::vec2 scale = {3, 3};

  void Render() override {
    // draw points
    shaderProgram->Use();
    shaderProgram->SetUniform("u_resolution", glUniform2f, this->WIDTH,
                              this->HEIGHT);

    shaderProgram->SetUniform("u_fractal_constant", glUniform2f, -0.8, 0.18);
    shaderProgram->SetUniform("u_fractal_iteration", glUniform1i,
                              fractal_iteration/2*2);
    // fractal_iteration=(fractal_iteration+1)%36;
    fractal_iteration ++;
    shaderProgram->SetUniform("u_shift", glUniform2f, 0.0, 0.0);
    shaderProgram->SetUniform("u_scale", glUniform2f, scale.x, scale.y);
    scale *= 0.98;

    // Draw the constellation
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, data.size() / 2);
    glBindVertexArray(0);
    if (pic_iteration < 72)
      SaveImage("images", "picture_" + std::to_string(pic_iteration));
    pic_iteration++;
  }

  ~JuliaSetScene() {
#ifndef NDEBUG
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }
};
#endif // JS_SCENE_HPP