#include "icosahedron.hpp"
#include "scene.hpp"
#include "shader_program.hpp"
#include <cmath>

class P6Scene : public Scene {
  std::unique_ptr<Make_ShaderProgram<5>::type> shaderProgram;
  std::unique_ptr<Icosahedron> icosahedron;
  int const pic_count = 360;
  int pic_iteration = 0;
  int parameter_tc = 1;

public:
  P6Scene(std::string const &windowTitle, int width, int height)
      : Scene(windowTitle, width, height) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  void InitScene() override {
    shaderProgram = std::make_unique<Make_ShaderProgram<5>::type>(
        Shader("shaders/vertex.glsl", GL_VERTEX_SHADER),
        Shader("shaders/tess_control.glsl", GL_TESS_CONTROL_SHADER),
        Shader("shaders/tess_eval.glsl", GL_TESS_EVALUATION_SHADER),
        Shader("shaders/geometry.glsl", GL_GEOMETRY_SHADER),
        Shader("shaders/fragment.glsl", GL_FRAGMENT_SHADER));
    icosahedron = std::make_unique<Icosahedron>();
  }

  void Render() override {
    // draw points
    shaderProgram->use();

    float angle = 2.0 * M_PI * static_cast<float>(pic_iteration) /
                  static_cast<float>(pic_count);
    pic_iteration = (pic_iteration + 1) % pic_count;
    glm::mat4 matModel(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    matModel = glm::rotate(matModel, angle, glm::vec3(1, 1, 0));
    shaderProgram->setValue("matModel", glUniformMatrix4fv, 1, GL_FALSE,
                            &matModel[0][0]);
    if (pic_iteration == 0)
      ++parameter_tc;
    shaderProgram->setValue("parameter_tc", glUniform1i, 1);

    icosahedron->Draw();
  }

  ~P6Scene() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

int main() {
  P6Scene scene("P6", 800, 800);
  scene.RenderLoop();
  return 0;
}