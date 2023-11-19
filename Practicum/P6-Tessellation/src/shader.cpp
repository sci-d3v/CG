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
  }
  GLchar const *shaderCode = code.c_str();
  // std::cout<< shaderCode<<std::endl;

  // 2. compile a shader
  Status();
  id = glCreateShader(shaderType);
  glShaderSource(id, 1, &shaderCode, nullptr);
  glCompileShader(id);
}

Shader::~Shader() {
  Status();
  glDeleteShader(id);
  Status();
}

std::string Shader::ShaderName(GLuint shaderType) {
  switch (shaderType) {
  case GL_VERTEX_SHADER:
    return "GL_VERTEX_SHADER";
  case GL_TESS_CONTROL_SHADER:
    return "GL_TESS_CONTROL_SHADER";
  case GL_TESS_EVALUATION_SHADER:
    return "GL_TESS_EVALUATION_SHADER";
  case GL_GEOMETRY_SHADER:
    return "GL_GEOMETRY_SHADER";
  case GL_FRAGMENT_SHADER:
    return "GL_FRAGMENT_SHADER";
  case GL_COMPUTE_SHADER:
    return "GL_COMPUTE_SHADER";
  default:
    return "UNKNOWN_SHADER";
  }
}

void Shader::ParameterStatus(GLuint pname, std::string const &name) {
  // #define VAR_NAME(x) #x
  // std::cout << VAR_NAME(pname);
  GLint params;
  glGetShaderiv(id, pname, &params);
  if (params && name == "GL_SHADER_TYPE")
    std::cout << ShaderName(params);
  else
    std::cout << name;
  if (params) {
    std::cout << "::SUCCESS " << std::endl;
  } else {
    std::cout << "::ERROR" << std::endl;
  }

  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &params);
  std::cout << "INFO_LOG_LENGTH: " << params << std::endl;
  if (params) {
    GLchar log[params + 1];
    glGetShaderInfoLog(id, params, nullptr, log);
    std::cout << log;
  }
}

#ifndef GET_MACRO_NAME
#define GET_MACRO_NAME(name) name, #name
#endif
/*
  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShader.xhtml
*/
// utility function for checking status shader.
void Shader::Status() {
  std::cout << "Shader id: " << id << std::endl;
  // ParameterStatus(GL_SHADER_TYPE, "GL_SHADER_TYPE");
  ParameterStatus(GET_MACRO_NAME(GL_SHADER_TYPE));
  ParameterStatus(GET_MACRO_NAME(GL_COMPILE_STATUS));
  ParameterStatus(GET_MACRO_NAME(GL_DELETE_STATUS));
}