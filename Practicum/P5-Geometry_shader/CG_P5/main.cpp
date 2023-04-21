#include <cstdio>
#include <string>

#include <cstdarg>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

const unsigned int WIDTH = 800, HEIGHT = 600;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

GLchar *glShaderLoad(char *path, int *length);

void getShader(GLuint *shader, char *path, int typeShader);

void getShaderProgram(GLuint *shaderProgram, int n, ...);

int main()
{
    // Init GLFW
    if (!glfwInit())
        exit(EXIT_FAILURE);
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFW window object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG_P5", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, keyCallback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to set up the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    GLuint vertexShader;
    getShader(&vertexShader, (char *)"shaders/vertex.glsl", GL_VERTEX_SHADER);
    GLuint geometryShader;
    getShader(&geometryShader, (char *)"shaders/geometry.glsl", GL_GEOMETRY_SHADER);
    GLuint fragmentShader;
    getShader(&fragmentShader, (char *)"shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    GLuint shaderProgram;
    getShaderProgram(&shaderProgram, 3, &vertexShader, &geometryShader, &fragmentShader);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float points[] = {
        .0f, .0f, .0f, 0.0f, 0.0f, 0.0f,
        1.0f, .0f, .0f, 1.0f, 0.0f, 0.0f,
        .5f, .0f, sqrt(3.0f) / 2.0f, 0.0f, 1.0f, 0.0f,
        .5f, sqrt(2.0f) / sqrt(3.0f), sqrt(3.0f) / 6.0f, 0.0f, 0.0f, 1.0f};
    GLuint indices[] = {
        0, 1, 2,
        0, 1, 3,
        1, 2, 3,
        0, 2, 3};
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid *)0);
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid *)(3 * sizeof(float)));

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
        glUseProgram(shaderProgram);

        float angle = 0.5f * (float)glfwGetTime();

        glm::mat4 matModel(1.0f, 0.0f, 0.0f, -1.0f/2.0f,
                           0.0f, 1.0f, 0.0f, -sqrt(3.0f)/6.0f,
                           0.0f, 0.0f, 1.0f, -sqrt(3.0f)/6.0f,
                           0.0f, 0.0f, 0.0f, 1.0f);

        matModel = glm::rotate(matModel, angle, glm::vec3(0, 1, 0));

        GLint ulMatModel = glGetUniformLocation(shaderProgram, "matModel");
        glUniformMatrix4fv(ulMatModel, 1, GL_FALSE, &matModel[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, nullptr);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
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

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    glfwSetWindowSize(window, WIDTH, HEIGHT);
}

GLchar *
glShaderLoad(char *path, int *length)
{
    GLchar *shaderSource = nullptr;
    int descriptor = open(path, O_RDONLY);
    if (descriptor != -1)
    {
        FILE *file = fdopen(descriptor, "rb");
        if (file)
        {
            struct stat statistics;
            if (fstat(descriptor, &statistics) != -1)
            {
                shaderSource = (GLchar *)malloc(statistics.st_size);
                fread(shaderSource, sizeof(char), statistics.st_size, file);
                *length = (statistics.st_size);
            }
            fclose(file);
        }
        // close(descriptor);
    }
    //    printf("%s\n", path);
    //    printf("%s\n", shaderSource);
    return shaderSource;
}

void getShader(GLuint *shader, char *path, int typeShader)
{
    // Build and compile our shader program
    // Shader
    GLint length = 0;
    const GLchar *shaderSource = glShaderLoad(path, &length);
    *shader = glCreateShader(typeShader);
    glShaderSource(*shader, 1, &shaderSource, &length);
    glCompileShader(*shader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(*shader, 512, nullptr, infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
    }
}

void getShaderProgram(GLuint *shaderProgram, int n, ...)
{
    // Link shaders
    *shaderProgram = glCreateProgram();

    va_list argPtr;
    va_start(argPtr, n);
    for (int i = 0; i < n; ++i)
    {
        GLuint *shader = va_arg(argPtr, GLuint *);
        glAttachShader(*shaderProgram, *shader);
    }
    va_end(argPtr);
    glLinkProgram(*shaderProgram);

    GLint success;
    GLchar infoLog[512];
    // Check for linking errors
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(*shaderProgram, 512, nullptr, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    va_start(argPtr, n);
    for (int i = 0; i < n; ++i)
    {
        GLuint *shader = va_arg(argPtr, GLuint *);
        glDeleteShader(*shader);
    }
    va_end(argPtr);
}