#include "scene.hpp"
#include "shader.hpp"
#include "shader_program.hpp"
#include <cmath>

class P1Scene : public Scene {
  std::unique_ptr<Make_ShaderProgram<2>::type> shaderProgram;
  unsigned int VAO, VBO;
  // GLfloat data[8] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
  std::array<GLfloat, 8> data = {-1.0f, -1.0f, 1.0f, -1.0f,
                                 -1.0f, 1.0f,  1.0f, 1.0f};

public:
  P1Scene(std::string const &windowTitle, int width, int height)
      : Scene(windowTitle, width, height) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  void InitScene() override {
    shaderProgram = std::make_unique<Make_ShaderProgram<2>::type>(
        Shader("shaders/vertex.glsl", GL_VERTEX_SHADER),
        Shader("shaders/fragment.glsl", GL_FRAGMENT_SHADER));

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

  void Render() override {
    // draw points
    shaderProgram->Use();

    shaderProgram->SetUniform("u_resolution", glUniform2f, this->WIDTH,
                            this->HEIGHT);

    // Draw the constellation
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, data.size() / 2);
    glBindVertexArray(0);
  }

  ~P1Scene() {
    SaveImage("images", "picture");
#ifndef NDEBUG
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }
};

int main() {
  P1Scene scene("P1-test", 800, 800);
  // scene.initScene();
  scene.RenderLoop();
  return 0;
}