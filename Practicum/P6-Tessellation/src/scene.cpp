#include "scene.hpp"
#include "shader_program.hpp"
#include <memory>
#include <string>

Scene::Scene(std::string const &windowTitle, int width, int height)
    : WIDTH(width), HEIGHT(height) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  // Init GLFW
  if (!glfwInit())
    exit(EXIT_FAILURE);
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFW window object that we can use for GLFW's functions
  window =
      glfwCreateWindow(WIDTH, HEIGHT, windowTitle.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, KeyCallback);

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
}

Scene::~Scene() {
  glfwTerminate();
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void Scene::RenderLoop() {
  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  InitScene();
  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // render
    // ------
    glClearColor(.3f, .3f, .3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Render();
    glfwSwapBuffers(window);
    // Poll IO events (keys pressed/released, mouse moved etc.)
    glfwPollEvents();
  }
}

void Scene::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mode) {
  // Is called whenever a key is pressed/released via GLFW
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  // glfwSetWindowSize(window, WIDTH, HEIGHT);
}