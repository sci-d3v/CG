#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <memory>
#include <utility>

class Shader {
public:
  Shader(std::string const &path, GLuint shaderType);
  ~Shader();

  GLuint id = 0;
};
#endif // SHADER_HPP
