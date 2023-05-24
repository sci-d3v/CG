#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include "shader.hpp"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <memory>
#include <utility>

template <typename... Args> class ShaderProgram {
public:
  // constructor generates the shader on the fly
  ShaderProgram(Args const &...args) {
    // Link shaders
    id = glCreateProgram();
    std::tuple<Args...> shaders = std::make_tuple(args...);
    std::apply([&](auto &&...args) { (glAttachShader(id, args.id), ...); },
               shaders);

    glLinkProgram(id);
    Status();
  }

  // activate the shader program
  void use() const { glUseProgram(id); }

  /*
    Instead of
      GLint ulMatModel = glGetUniformLocation(shaderProgram, "matModel");
      glUniformMatrix4fv(ulMatModel, 1, GL_FALSE, &matModel[0][0]);
  */
  // utility uniform functions
  template <typename F, typename... Types>
  void setValue(std::string const &name, F f, Types &&...values) const {
    f(glGetUniformLocation(id, name.c_str()), std::forward<Types>(values)...);
  }

  ~ShaderProgram() { Status(); };

private:
  GLuint id = 0;

#define VAR_NAME(x) #x
  // utility function for checking shader linking errors.
  void Status() const {
    GLint params;
    glGetShaderiv(id, GL_LINK_STATUS, &params);
    std::cout << VAR_NAME(GL_LINK_STATUS);
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
};

/*
  You can use Make_ShaderProgram<N>::type
  instead of ShaderProgram<Shader, Shader, ..., N>
*/
template <typename H, typename T> struct ShaderNumber;
template <typename H, typename... T>
struct ShaderNumber<H, ShaderProgram<T...>> {
  using type = ShaderProgram<H, T...>;
};

template <int N> struct Make_ShaderProgram {
  using type =
      typename ShaderNumber<Shader,
                            typename Make_ShaderProgram<N - 1>::type>::type;
};

template <> struct Make_ShaderProgram<0> {
  using type = ShaderProgram<>;
};

#endif // SHADER_PROGRAM_HPP