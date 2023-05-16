#include "shader_program.hpp"

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>

const unsigned int WIDTH = 800, HEIGHT = 800;

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFW window object that we can use for GLFW's functions
  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "CG_P6", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, keyCallback);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to set up the OpenGL Function pointers
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  auto shaderProgram =
      Make_ShaderProgram<5>::type(Shader("shaders/vertex.glsl", GL_VERTEX_SHADER),
         Shader("shaders/geometry.glsl", GL_GEOMETRY_SHADER),
         Shader("shaders/tess_control.glsl", GL_TESS_CONTROL_SHADER),
         Shader("shaders/tess_eval.glsl", GL_TESS_EVALUATION_SHADER),
         Shader("shaders/fragment.glsl", GL_FRAGMENT_SHADER));
 
 
 // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

#define X .525731112119133606
#define Z .850650808352039932

    static GLfloat points[] = {
        -X, 0.0, Z, X, 0.0, Z, -X, 0.0, -Z, X, 0.0, -Z,
        0.0, Z, X, 0.0, Z, -X, 0.0, -Z, X, 0.0, -Z, -X,
        Z, X, 0.0, -Z, X, 0.0, Z, -X, 0.0, -Z, -X, 0.0};

    // These are the 20 faces.  Each of the three entries for each
    // vertex gives the 3 vertices that make the face.
    static GLint indices[] = {
        0, 4, 1, 0, 9, 4, 9, 5, 4, 4, 5, 8, 4, 8, 1,
        8, 10, 1, 8, 3, 10, 5, 3, 8, 5, 2, 3, 2, 7, 3,
        7, 10, 3, 7, 6, 10, 7, 11, 6, 11, 0, 6, 0, 1, 6,
        6, 1, 10, 9, 0, 11, 9, 11, 2, 9, 2, 5, 7, 2, 11};

    unsigned int VBO, VAO, IBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid *)0);

    glBindVertexArray(0);

    // FPS-related variables (Frame per second)
    const double FPS = 1.0 / 30.0;
    double startTime = glfwGetTime(), currentTime;
    // SRR-related variables (Screen refresh rate)
    const double SRR = 2.0;
    double endTime = 0.0, deltaTime = 0.0;
    int frameCount = 0;
    // render loop
    // -----------

    while (!glfwWindowShouldClose(window))
    {

        // Update time delay according to FPS
        do
        {
            currentTime = glfwGetTime();
        } while (currentTime - startTime < FPS);
        startTime = currentTime;

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw points
        shaderProgram.use();

        float angle = M_PI * 0.25 * (float)glfwGetTime();

        glm::mat4 matModel(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f);

        matModel = glm::rotate(matModel, angle, glm::vec3(0, 1, 0));

        shaderProgram.setValue("matModel", glUniformMatrix4fv, 1, GL_FALSE, &matModel[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_PATCHES, sizeof(indices), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Message update time delay according to SRR
        frameCount++;
        deltaTime += glfwGetTime() - startTime;
        if (startTime - endTime > SRR)
        {
            printf("setFPS: %5.2f \trealFPS: %5.2f \tframeTime: %5.8f\n",
                   1.0 / FPS, frameCount / (startTime - endTime), deltaTime / frameCount);
            endTime = startTime;
            deltaTime = 0;
            frameCount = 0;
        }
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
 
  return 0;
}