#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdio>

#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

#include <glm/glm.hpp>
#include <string>
#include <vector>

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

const GLuint WIDTH = 800, HEIGHT = 800;
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);

GLchar* glShaderLoad(const char* path);

int main(int argc, char* argv[]) {
  // Init GLFW
  glfwInit();
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CG_P2", NULL, NULL);
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  glewInit();

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  // Build and compile our shader program
  // Vertex shader
  const GLchar* vertexShaderSource = glShaderLoad("shaders/vertex.glsl");
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // Check for compile time errors
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
  }

  // Fragment shader
  const GLchar* fragmentShaderSource = glShaderLoad("shaders/fragment.glsl");
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // Check for compile time errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
  }

  // Link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // Check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLuint VBO, VAO;
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

  glBindVertexArray(0);  // Unbind starVAO

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
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  glfwSetWindowSize(window, WIDTH, HEIGHT);
}

// Параметры, с которыми производились расчеты, лежат в заголовочном файле
// нужно возвращать ошибку!
GLchar* glShaderLoad(const char* path) {
  GLchar* shaderSource = NULL;

  int descriptor = open(path, O_RDONLY);

  if (descriptor != -1) {
    FILE* file = fdopen(descriptor, "rb");

    if (file) {
      struct stat statistics;

      if (fstat(descriptor, &statistics) != -1) {
        shaderSource = (GLchar*)malloc(statistics.st_size);
        fread(shaderSource, sizeof(char), statistics.st_size, file);
      }
      fclose(file);
    }
    close(descriptor);
  }
  printf("%s\n", path);
  // printf("%s\n", shaderSource);
  return shaderSource;
}
