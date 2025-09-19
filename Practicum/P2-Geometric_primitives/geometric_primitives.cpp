#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const GLuint WIDTH = 800, HEIGHT = 800;

// Define a struct to hold both position and color for each vertex
struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
};

// Now define the vertices with their positions and colors
std::vector<Vertex> vertices = {{{{-0.5f, -0.2f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{-0.3f, -0.3f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{0.0f, -0.1f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{0.2f, 0.0f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{0.6f, 0.1f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{0.5f, 0.4f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{0.2f, 0.3f, 0.0f}, {0.5f, 1.0f, 0.5f}},
                                 {{0.2f, 0.0f, 0.0f}, {0.5f, 1.0f, 0.5f}},

                                 {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{-0.25f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{0.25f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{0.8f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{0.4f, -0.05f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{0.6f, -0.6f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{0.0f, -0.2f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{-0.6f, -0.6f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{-0.4f, -0.05f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{-0.8f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                                 {{-0.25f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};

int main(int argc, char* argv[]) try {
  // Init GLFW
  if (!glfwInit())
    throw std::runtime_error("GLFW::INITIALIZATION_FAILED");

  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow* window =
      glfwCreateWindow(WIDTH, HEIGHT, "CG_example", NULL, NULL);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("GLFW::WINDOW_CREATION_FAILED");
  }
  glfwMakeContextCurrent(window);

  // Is called whenever a key is pressed/released via GLFW
  auto key_callback = [](GLFWwindow* window, int key, int scancode, int action,
                         int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    glfwSetWindowSize(window, WIDTH, HEIGHT);
  };

  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error("GLFW::INITIALIZATION_FAILED");
  }

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  // Retrieve a shader source code from a file
  auto LoadShader = [](const char* path, std::string& shaderCode) {
    std::ifstream fileStream;
    // ensure ifstream objects can throw exceptions:
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      // open files
      fileStream.open(path);
      std::stringstream stringStream;
      // read file's buffer contents into streams
      stringStream << fileStream.rdbuf();
      // close file handlers
      fileStream.close();
      // convert stream into string
      shaderCode = stringStream.str();
    } catch (std::ifstream::failure& e) {
      throw std::runtime_error("SHADER::FILE_NOT_SUCCESSFULLY_READ: " +
                               std::string(e.what()) + " " + path);
    }
  };

  // Build and compile shader
  auto GetShader = [&](GLuint* shaderID, const char* path, int typeShader) {
    std::string shaderCode;
    LoadShader(path, shaderCode);
    *shaderID = glCreateShader(typeShader);
    const char* vShaderCode = shaderCode.c_str();
    glShaderSource(*shaderID, 1, &vShaderCode, NULL);
    glCompileShader(*shaderID);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(*shaderID, 512, NULL, infoLog);
      throw std::runtime_error("SHADER::COMPILATION_FAILED: " +
                               std::string(infoLog));
    }
  };

  GLuint vertexShaderID;
  GetShader(&vertexShaderID, "shaders/vertex.glsl", GL_VERTEX_SHADER);
  GLuint fragmentShaderID;
  GetShader(&fragmentShaderID, "shaders/fragment.glsl", GL_FRAGMENT_SHADER);

  auto GetShaderProgram = [](GLuint& shaderProgram, auto... shaders) {
    // Create shader program
    shaderProgram = glCreateProgram();

    // Lambda to attach shaders
    auto attachShaders = [&](GLuint shader) {
      glAttachShader(shaderProgram, shader);
    };

    // Apply to each shader
    (attachShaders(shaders), ...);

    // Link the shader program
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      throw std::runtime_error("SHADER::PROGRAM::LINKING_FAILED: " +
                               std::string(infoLog));
    }

    // Delete shaders after linking
    (glDeleteShader(shaders), ...);
  };

  GLuint shaderProgram;
  GetShaderProgram(shaderProgram, vertexShaderID, fragmentShaderID);

  GLuint VBO, VAO;
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and
    // attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (GLvoid*)(sizeof(vertices[0].position)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
  }

  // Game loop
  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    GLint translationLocation =
        glGetUniformLocation(shaderProgram, "translation");
    GLint scaleLocation = glGetUniformLocation(shaderProgram, "scale");
    GLint angleLocation = glGetUniformLocation(shaderProgram, "angle");

    glUniform3f(translationLocation, 0.0f, 0.0f, 0.0f);
    glUniform3f(scaleLocation, 1.0f, 1.0f, 1.0f);
    glUniform1f(angleLocation, 0.0f * M_PI);

    // Draw the vertices
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, 8);
    glBindVertexArray(0);

    // Draw the stars
    GLfloat scaleArray[] = {0.05f, 0.04f, 0.04f, 0.06f, 0.05f, 0.03f, 0.04f};
    GLfloat angleArray[] = {-0.03f, 0.08f,  -0.03f, 0.08f,
                            0.03f,  -0.05f, -0.06f};
    for (int position = 0; position < 7; ++position) {
      glUniform3f(translationLocation, vertices[position].position.x,
                  vertices[position].position.y, vertices[position].position.z);
      glUniform3f(scaleLocation, scaleArray[position], scaleArray[position],
                  1.0f);
      glUniform1f(angleLocation, angleArray[position] * M_PI);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLE_FAN, 8, 12);
      glBindVertexArray(0);
    }

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();
  return 0;
} catch (const std::exception& e) {
  std::cout << "ERROR::" << e.what() << std::endl;
  glfwTerminate();
  return 1;
}
