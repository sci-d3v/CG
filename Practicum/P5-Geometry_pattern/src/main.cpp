#include "application.hpp"

int main(int argc, char *argv[]) {
  Application application("Geometry Pattern", 800, 800);
  application.RenderLoop();
  return 0;
}
