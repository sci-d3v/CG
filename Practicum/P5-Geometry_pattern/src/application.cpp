#include "application.hpp"
#include "shader_program_impl.hpp"
#include <memory>
#include <string>

Application::~Application() {
  glDeleteTextures(1, &imageTexture);
  glfwTerminate();
}

Application::Application(std::string const &windowTitle, int width, int height)
    : WIDTH(width), HEIGHT(height), quadVAO(0) {
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
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

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

  // Turn off vertical syncronization
  glfwSwapInterval(1);
}

void Application::InitApplication() {
  computeSP = std::make_unique<Make_ShaderProgram<1>::type>(
      Shader("shaders/geometry_pattern_cs.glsl", GL_COMPUTE_SHADER));
  renderSP = std::make_unique<Make_ShaderProgram<2>::type>(
      Shader("shaders/render_vs.glsl", GL_VERTEX_SHADER),
      Shader("shaders/render_fs.glsl", GL_FRAGMENT_SHADER));

  InitQuad();
  InitTextures();
  glClearColor(1, 1, 1, 1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::RenderLoop() {
  // configure global OpenGL state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  InitApplication();
  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // render
    // ------
    glClearColor(.5f, .5f, .5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Render();
    glfwSwapBuffers(window);
    // Poll IO events (keys pressed/released, mouse moved etc.)
    glfwPollEvents();
  }
}

void Application::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void Application::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mode) {
  // Is called whenever a key is pressed/released via GLFW
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

void Application::InitQuad() {
  // Each vertex consists of
  // positions (3 elements),
  // texture coordinates (2 elements).
  float quadVertices[] = {
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };
  // setup plane VAO
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
}

void Application::InitTextures() {
  // Create imageTexture for OpenGL operation
  // -----------------------------------
  {
    glGenTextures(1, &imageTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA,
                 GL_FLOAT, NULL);

    glBindImageTexture(0, imageTexture, 0, GL_FALSE, 0, GL_READ_WRITE,
                       GL_RGBA32F);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glClearTexImage(imageTexture, 0, GL_RGBA, GL_FLOAT, nullptr);
  }
}

void Application::Render() {
  // render image to quad
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  computeSP->use();
  computeSP->setValue("texture_size", glUniform2f, static_cast<float>(WIDTH),
                      static_cast<float>(HEIGHT));

  glDispatchCompute((unsigned int)WIDTH / 10, (unsigned int)HEIGHT / 10, 1);
  // make sure writing to image has finished before read
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  // render Quad
  renderSP->use();
  renderSP->setValue("imageTexture", glUniform1i, 0);
  renderSP->setValue("texture_width", glUniform1f, static_cast<float>(WIDTH));
  renderSP->setValue("texture_height", glUniform1f, static_cast<float>(HEIGHT));
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
