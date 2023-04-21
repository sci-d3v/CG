#include "shader.h"
#include "mesh.h"
#include "model.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"


const unsigned int WIDTH = 800, HEIGHT = 600;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

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
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG_P4", nullptr, nullptr);
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
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // build and compile shaders
    // -------------------------
    Shader planetShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    // load models
    // -----------
    Model planetModel("objects/Planet.obj");


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        planetShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(70.0f),
                                                (float)WIDTH / HEIGHT, 0.3f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        
        float angle = M_PI * 0.25 * (float)glfwGetTime();
        model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

        
        glm::mat4 mv = view * model;
        planetShader.setMat4("ModelViewMatrix", mv);
        planetShader.setMat3("NormalMatrix",
                    glm::mat3( glm::vec3(mv[0]), 
                    glm::vec3(mv[1]), glm::vec3(mv[2]) ));
        planetShader.setMat4("ProjectionMatrix", projection);
        planetShader.setMat4("MVP", projection * mv);

        planetShader.setVec3("Light.Intensity", glm::vec3(0.9f,0.9f,0.9f) );
        planetShader.setVec4("Light.Position", view * glm::vec4(10.0f * cos(angle),1.0f,10.0f * sin(angle),1.0f) );
        planetShader.setVec3("Material.Ks", 0.2f, 0.2f, 0.2f);
        planetShader.setVec3("Material.Ka", 0.1f, 0.1f, 0.1f);
        planetShader.setFloat("Material.Shininess", 1.0f);


        planetModel.Draw(planetShader.ID);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
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