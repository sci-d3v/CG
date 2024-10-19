#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Define a struct to hold both position and color for each vertex
struct Vertex {
  glm::vec3 position;
};

// Now define the vertices with their positions and colors
std::vector<Vertex> vertices = {
    {{0.0f, -0.5f, 0.0f}},
    {{0.0f, 0.5f, 0.0f}},
};

std::vector<GLuint> indices = {0, 1};

const unsigned int WIDTH = 800, HEIGHT = 600;

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  glfwSetWindowSize(window, WIDTH, HEIGHT);
}

int main() {
  // Init GLFW
  if (!glfwInit())
    exit(EXIT_FAILURE);
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFW window object that we can use for GLFW's functions
  GLFWwindow* window =
      glfwCreateWindow(WIDTH, HEIGHT, "CG_P4_Fractal", nullptr, nullptr);
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

  GLuint vertexShader;
  GetShader(&vertexShader, (char*)"shaders/vertex.glsl", GL_VERTEX_SHADER);
  GLuint geometryShader;
  GetShader(&geometryShader, (char*)"shaders/geometry.glsl",
            GL_GEOMETRY_SHADER);
  GLuint fragmentShader;
  GetShader(&fragmentShader, (char*)"shaders/fragment.glsl",
            GL_FRAGMENT_SHADER);

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
  GetShaderProgram(shaderProgram, vertexShader, geometryShader, fragmentShader);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // Set up vertex data (and buffer(s)) and attribute pointers
  GLuint VBO, VAO, IBO;
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Unbind VAO
    glBindVertexArray(0);
  }

  // FPS-related variables (Frame per second)
  const double FPS = 1.0 / 30.0;
  double startTime = glfwGetTime(), currentTime;
  // SRR-related variables (Screen refresh rate)
  const double SRR = 2.0;
  double endTime = 0.0, deltaTime = 0.0;
  int frameCount = 0;
  // render loop
  // -----------
  int recursion_steps = 0;
  while (!glfwWindowShouldClose(window)) {

    // Update time delay according to FPS
    do {
      currentTime = glfwGetTime();
    } while (currentTime - startTime < FPS);
    startTime = currentTime;

    // render
    // ------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw points
    glUseProgram(shaderProgram);

    float angle = 0.5f * (float)glfwGetTime();

    glm::mat4 matModel = glm::mat4(1.0f);

    matModel = glm::rotate(matModel, angle, glm::vec3(0, 1, 0));

    GLint ulMatModel = glGetUniformLocation(shaderProgram, "matModel");
    glUniformMatrix4fv(ulMatModel, 1, GL_FALSE, &matModel[0][0]);

    GLint ulRecursionSteps =
        glGetUniformLocation(shaderProgram, "recursion_steps");
    glUniform1i(ulRecursionSteps, recursion_steps%2);

    glBindVertexArray(VAO);
    glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, nullptr);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Message update time delay according to SRR
    frameCount++;
    deltaTime += glfwGetTime() - startTime;
    if (startTime - endTime > SRR) {
      // printf("setFPS: %5.2f \trealFPS: %5.2f \tframeTime: %5.8f\n",
      //        1.0 / FPS, frameCount / (startTime - endTime), deltaTime / frameCount);
      endTime = startTime;
      deltaTime = 0;
      frameCount = 0;

      ++recursion_steps;
    }
  }

  // Properly de-allocate all resources once they've outlived their purpose
  // 1. VAO, VBO and IBO
  glDeleteBuffers(1, &IBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  // 2. Shader Program
  glDeleteProgram(shaderProgram);
  // 3. Terminate GLFW, clearing any resources allocated by GLFW.
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
