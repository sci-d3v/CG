#ifndef CUBE_HPP
#define CUBE_HPP

#include <array>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <memory>
#include <utility>

#include <GL/glew.h>

class Cube {
private:
  unsigned int VAO, VBO, VBOColor, IBO;
  
  // There are the 8 points, where each point consists of 3 coordinates.
  std::array<float, 24> points = {
    1.0, 1.0, 1.0,
    -1.0, 1.0, 1.0,
    -1.0, -1.0, 1.0,
    1.0, -1.0, 1.0,

    1.0, 1.0, -1.0,
    -1.0, 1.0, -1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0
    };

  std::array<float, 24> points_color = {
    1.0, 1.0, 1.0,
    -1.0, 1.0, 1.0,
    -1.0, -1.0, 1.0,
    1.0, -1.0, 1.0,

    1.0, 1.0, -1.0,
    -1.0, 1.0, -1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0
  };

  // These are the 20 faces.  Each of the three entries for each vertex gives
  // the 3 vertices that make the face.
  std::array<int, 24> indices = {
      0,1,2,3,
      4,5,6,7,
      0,1,5,4,
      1,2,6,5,
      2,3,7,6,
      3,0,4,7
      };

public:
  Cube();
  void Draw() const;
  ~Cube();
};
#endif // CUBE_HPP