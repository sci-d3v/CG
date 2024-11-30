#include "icosahedron.hpp"
#include <GL/glew.h>
#include <cstddef>
#include <iostream>

Icosahedron::Icosahedron() {
  // Set up vertex data (and buffer(s)) and configure vertex attributes
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &VBOColor);
  glGenBuffers(1, &IBO);

  glBindVertexArray(VAO);
  // Position attribute
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);
  // Color attribute
  glBindBuffer(GL_ARRAY_BUFFER, VBOColor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points_color), points_color.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

#ifndef NDEBUG
  std::cout << "indices: counts " << indices.size() << "; size of "
            << sizeof(indices) << std::endl;
  std::cout << "points: counts " << points.size() << "; size of "
            << sizeof(points) << std::endl;
  std::cout << this << " The icosahedron was created" << std::endl;
#endif
}

void Icosahedron::Draw() const {
  glBindVertexArray(VAO);
  glDrawElements(GL_PATCHES, sizeof(indices), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

Icosahedron::~Icosahedron() {
  // optional: de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &VBOColor);
  glDeleteBuffers(1, &IBO);
#ifndef NDEBUG
  std::cout << this << " The icosahedron was destroyed" << std::endl;
#endif
}