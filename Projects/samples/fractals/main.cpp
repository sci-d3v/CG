#include "scene.hpp"
#include "shader.hpp"
#include "shader_program.hpp"
#include <cmath>
#include <glm/fwd.hpp>

#include "js_scene.hpp"
#include "ms_scene.hpp"

int main() {
  Scene *scene;
  if (1) {
    scene = new MandelbrotSetScene("Mandelbrot Set", 800, 800);
  } else {
    scene = new JuliaSetScene("Julia Set", 800, 800);
  }
  scene->RenderLoop();
  delete scene;
  return 0;
}