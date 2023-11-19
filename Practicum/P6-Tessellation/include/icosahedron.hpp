#ifndef ICOSAHEDRON_HPP
#define ICOSAHEDRON_HPP

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

class Icosahedron {
private:
  unsigned int VAO, VBO, VBOColor, IBO;

  float const X = .525731112119133606;
  float const Z = .850650808352039932;
  
  // There are the 12 points, where each point consists of 3 coordinates.
  std::array<float, 36> points = {
    -X,  0.0, Z,   
    X,   0.0, Z,   
    -X,  0.0, -Z,
    X,   0.0, -Z,  

    0.0, Z,   X,   
    0.0, Z,   -X,
    0.0, -Z,  X,   
    0.0, -Z,  -X,  

    Z,   X,   0.0,
    -Z,  X,   0.0, 
    Z,   -X,  0.0, 
    -Z,  -X,  0.0};

  std::array<float, 36> points_color = {
    0, 0.5, 1,
    1, 0.5, 1,
    0, 0.5, 0,
    1, 0.5, 0,

    0.5,1, 1,
    0.5,1,0,
    0.5,0,1,
    0.5, 0,0,

    1,1, 0.5,
    0,1,0.5,
    1,0,0.5,
    0, 0,0.5

    // 1,0,0, 
    // 0,1,0,
    // 0,0,1,

    // 1,1,0,
    // 1,0,1,
    // 0,1,1,

    // 1,0,0, 
    // 0,1,0,
    // 0,0,1,

    // 1,1,0,
    // 1,0,1,
    // 0,1,1
  };

  // These are the 20 faces.  Each of the three entries for each vertex gives
  // the 3 vertices that make the face.
  std::array<int, 60> indices = {
      0,  4, 1, 0, 9, 4, 9, 5,  4, 4, 5,  8,  4,  8, 1, 8,  10, 1,  8, 3,
      10, 5, 3, 8, 5, 2, 3, 2,  7, 3, 7,  10, 3,  7, 6, 10, 7,  11, 6, 11,
      0,  6, 0, 1, 6, 6, 1, 10, 9, 0, 11, 9,  11, 2, 9, 2,  5,  7,  2, 11};

public:
  Icosahedron();
  void Draw() const;
  ~Icosahedron();
};
#endif // ICOSAHEDRON_HPP