#include "cube.hpp"
#include "icosahedron.hpp"
#include "scene.hpp"
#include "shader_program.hpp"
#include <cmath>

class P6Scene : public Scene {
  std::unique_ptr<Make_ShaderProgram<5>::type> shaderProgram;
  std::unique_ptr<Icosahedron> icosahedron;
  int const pic_count = 36;
  int pic_iteration = 0;
  int parameter_tc = 1;
  std::string imagePath;

public:
  P6Scene(std::string const &windowTitle, int width, int height)
      : Scene(windowTitle, width, height) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  void InitScene() override {
    shaderProgram = std::make_unique<Make_ShaderProgram<5>::type>(
        Shader("shaders/vertex.glsl", GL_VERTEX_SHADER),
        Shader("shaders/tess_control.glsl", GL_TESS_CONTROL_SHADER),
        Shader("shaders/tess_evaluation.glsl", GL_TESS_EVALUATION_SHADER),
        Shader("shaders/geometry.glsl", GL_GEOMETRY_SHADER),
        Shader("shaders/fragment.glsl", GL_FRAGMENT_SHADER));
    icosahedron = std::make_unique<Icosahedron>();
  }

  void Render() override {
    // draw points
    shaderProgram->use();

    float angle = 2.0 * M_PIf * static_cast<float>(pic_iteration) /
                  static_cast<float>(pic_count);
    pic_iteration = (pic_iteration + 1) % pic_count;
    glm::mat4 matModel(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    matModel = glm::rotate(matModel, angle, glm::vec3(1, 1, 0));
    shaderProgram->setValue("matModel", glUniformMatrix4fv, 1, GL_FALSE,
                            &matModel[0][0]);
    if (pic_iteration == 0)
      ++parameter_tc;
    shaderProgram->setValue("parameter_tc", glUniform1i, parameter_tc);

    icosahedron->Draw();

    SaveImage("images", "picture_", pic_iteration);
  }

  ~P6Scene() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

int main(int argc, char *argv[]) {
  P6Scene scene("Icosahedron tessellation", 800, 800);
  // scene.initScene();
  scene.RenderLoop();
  return 0;
}