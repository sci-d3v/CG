#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>
#include <glm/glm.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Define a struct to hold both position and color for each vertex
struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 texture_coordinates;
};

// Now define the vertices with their positions and colors
std::vector<Vertex> vertices = {{
    // Star
    {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {360.f / 720.f, 360.f / 720.f}},
    {{-0.25f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {257.f / 720.f, 329.f / 720.f}},
    {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {200.f / 720.f, 146.f / 720.f}},
    {{0.25f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {355.f / 720.f, 260.f / 720.f}},
    {{0.8f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {520.f / 720.f, 146.f / 720.f}},
    {{0.4f, -0.05f, 0.0f}, {1.0f, 1.0f, 1.0f}, {456.f / 720.f, 330.f / 720.f}},
    {{0.6f, -0.6f, 0.0f}, {1.0f, 1.0f, 1.0f}, {613.f / 720.f, 446.f / 720.f}},
    {{0.0f, -0.2f, 0.0f}, {1.0f, 1.0f, 1.0f}, {415.f / 720.f, 446.f / 720.f}},
    {{-0.6f, -0.6f, 0.0f}, {1.0f, 1.0f, 1.0f}, {352.f / 720.f, 627.f / 720.f}},
    {{-0.4f, -0.05f, 0.0f}, {1.0f, 1.0f, 1.0f}, {293.f / 720.f, 446.f / 720.f}},
    {{-0.8f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {92.f / 720.f, 446.f / 720.f}},
    // Background
    {{1.f, 1.f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f}},
    {{1.f, -1.f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.f, 1.f}},
    {{-1.f, -1.f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f}},
    {{-1.f, 1.f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f}},
}};

std::vector<GLuint> indices = {
    // Star
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1,
    // Background
    11, 12, 13, 14};

const int WIDTH = 960, HEIGHT = 540;

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mode);

int main(int argc, char* argv[]) {
  // Init GLFW
  if (!glfwInit())
    exit(EXIT_FAILURE);
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFW window object that we can use for GLFW's functions
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CG_P3", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, keyCallback);

  // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to set up the OpenGL Function pointers
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    exit(EXIT_FAILURE);
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
      std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
                << std::endl
                << path << std::endl;
      exit(EXIT_FAILURE);
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
      std::cerr << "ERROR::SHADER::COMPILATION_FAILED: " << infoLog
                << std::endl;
      exit(EXIT_FAILURE);
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
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
      exit(EXIT_FAILURE);
    }

    // Delete shaders after linking
    (glDeleteShader(shaders), ...);
  };

  GLuint shaderProgram;
  GetShaderProgram(shaderProgram, vertexShaderID, fragmentShaderID);

  // Set up vertex data (and buffer(s)) and attribute pointers
  GLuint VBO, VAO, IBO;
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // Unbind VAO
    glBindVertexArray(0);
  }

  // Create and load a starTextureID
  auto loadTextureFromFile = [](const char* path, GLuint& textureID) {
    // Create a starTextureID
    glGenTextures(1, &textureID);
    // Image parameters
    int width, height;
    int channels = 0;

    unsigned char* data_image =
        SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_AUTO);
    if (*data_image) {
      glBindTexture(GL_TEXTURE_2D, textureID);
      // All upcoming GL_TEXTURE_2D operations now have effect on this starTextureID object
      // Set the starTextureID wrapping parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      // Set starTextureID wrapping to GL_REPEAT (usually basic wrapping method)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      // Set starTextureID filtering parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      GLenum format;
      switch (channels) {
        case 1:
          format = GL_RED;
          break;
        case 3:
          format = GL_RGB;
          break;
        case 4:
          format = GL_RGBA;
          break;
        default: {
          std::cerr << "The starTextureID failed to select a channel format: "
                    << path << std::endl;
          exit(EXIT_FAILURE);
        }
      }
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data_image);
      glGenerateMipmap(GL_TEXTURE_2D);
      SOIL_free_image_data(data_image);
      // Unbind starTextureID when done, so we won't accidentally mess up our starTextureID.
      glBindTexture(GL_TEXTURE_2D, 0);
    } else {
      std::cerr << "The starTextureID failed to load at path: " << path
                << std::endl;
      SOIL_free_image_data(data_image);
      exit(EXIT_FAILURE);
    }
  };

  GLuint starTextureID, planetTextureID;
  loadTextureFromFile("textures/window_star.png", starTextureID);
  loadTextureFromFile("textures/planets.png", planetTextureID);

  float starMove = 0;
  // Game loop
  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();
    // Render
    // Clear the colorful
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    GLint translationLocation =
        glGetUniformLocation(shaderProgram, "translation");
    GLint scaleLocation = glGetUniformLocation(shaderProgram, "scale");
    GLint angleLocation = glGetUniformLocation(shaderProgram, "angle");

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, planetTextureID);

    glUniform3f(translationLocation, 0.0f, 0.0f, 0.0f);
    glUniform3f(scaleLocation, 1.0f, 1.0f, 1.0f);
    glUniform1f(angleLocation, 0.0f * M_PI);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT,
                   (void*)(12 * sizeof(GLuint)));
    glBindVertexArray(0);

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, starTextureID);

    glUniform3f(translationLocation, 0.6f, -0.3f, 0.0f);
    glUniform3f(scaleLocation, 0.3f, 0.3f, 1.0f);
    glUniform1f(angleLocation, 0.25f * M_PI);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_FAN, 12, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    starMove = starMove > 2.f ? 0.f : starMove + 0.001f;

    glUniform3f(translationLocation, 1.f - starMove, 0.7f, 0.0f);
    glUniform3f(scaleLocation, 0.1f, 0.1f, 1.0f);
    glUniform1f(angleLocation, 0.6f * M_PI);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_FAN, 12, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Properly de-allocate all resources once they've outlived their purpose
  // 1. VAO, VBO and IBO
  glDeleteBuffers(1, &IBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  // 2. Textures
  glDeleteTextures(1, &planetTextureID);
  glDeleteTextures(1, &starTextureID);
  // 3. Shader Program
  glDeleteProgram(shaderProgram);
  // 4. Terminate GLFW, clearing any resources allocated by GLFW.
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  glfwSetWindowSize(window, WIDTH, HEIGHT);
}
