#include "shader.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>

Shader::Shader(std::string const &path, GLuint shaderType) {
  // 1. retrieve the shader source code from file
  std::string code;
  std::ifstream file;
  // ensure ifstream objects can throw exceptions:
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    file.open(path.c_str());
    std::stringstream shaderStream;
    // read file's buffer contents into streams
    shaderStream << file.rdbuf();
    // close file handlers
    file.close();
    // convert stream into string
    code = shaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
              << std::endl;
    exit(EXIT_FAILURE);
  }
  GLchar const *shaderCode = code.c_str();
// std::cout<< shaderCode<<std::endl;

// 2. compile a shader
  id = glCreateShader(shaderType);
  glShaderSource(id, 1, &shaderCode, nullptr);
  glCompileShader(id);
  // Check for compile time errors
  GLint success;
  GLchar infoLog[512];

  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED: " << infoLog << std::endl;
    exit(EXIT_FAILURE);
  }
}

Shader::~Shader() {
  glDeleteShader(id);
}
