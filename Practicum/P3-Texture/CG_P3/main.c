#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <SOIL/SOIL.h>


const int WIDTH = 960, HEIGHT = 540;

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
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG_P3", NULL, NULL);
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
    getShader(&vertexShader, "shaders/vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader;
    getShader(&fragmentShader, "shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    GLuint shaderProgram;
    getShaderProgram(&shaderProgram, 2, &vertexShader, &fragmentShader);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat starVertices[] = {
            // Positions (x,y,z) Colors (r,g,b) Texture position (x,y)
            0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 356.f/723.f, 361.f/720.f,
            -0.25f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 257.f/723.f, 329.f/720.f,
            0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 200.f/723.f, 146.f/720.f,
            0.25f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 355.f/723.f, 260.f/720.f,
            0.8f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 520.f/723.f, 146.f/720.f,
            0.4f, -0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 456.f/723.f, 330.f/720.f,
            0.6f, -0.6f, 0.0f, 1.0f, 1.0f, 1.0f, 613.f/723.f, 446.f/720.f,
            0.0f, -0.2f, 0.0f, 1.0f, 1.0f, 1.0f, 415.f/723.f, 446.f/720.f,
            -0.6f, -0.6f, 0.0f, 1.0f, 1.0f, 1.0f, 352.f/723.f, 627.f/720.f,
            -0.4f, -0.05f, 0.0f, 1.0f, 1.0f, 1.0f, 293.f/723.f, 446.f/720.f,
            -0.8f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 92.f/723.f, 446.f/720.f
    };
    GLuint starIndices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1};
    GLuint starVBO, starVAO, starIBO;
    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);
    glGenBuffers(1, &starIBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(starVertices), starVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(starIndices), starIndices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // Unbind starVAO
    glBindVertexArray(0);

    GLfloat planets[] = {
            // Positions          // Colors           // Texture Coords
            1.f, 1.f, 0.0f, 1.0f, 1.0f, 1.0f, 1.f, 0.f, // Top Right
            1.f, -1.f, 0.0f, 1.0f, 1.0f, 1.0f, 1.f, 1.f, // Bottom Right
            -1.f, -1.f, 0.0f, 1.0f, 1.0f, 1.0f, 0.f, 1.f, // Bottom Left
            -1.f, 1.f, 0.0f, 1.0f, 1.0f, 1.0f, 0.f, 0.f, // Top Left
    };

    GLuint planetVBO, planetVAO;
    glGenVertexArrays(1, &planetVAO);
    glGenBuffers(1, &planetVBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(planetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planets), planets, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Unbind planetVBO
    glBindVertexArray(0);

    // Load and create a texture
    GLuint starTexture;
    glGenTextures(1, &starTexture);
    glBindTexture(GL_TEXTURE_2D,
                  starTexture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT);    // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int width, height;
    unsigned char *image = SOIL_load_image("textures/window_star.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentally mess up our texture.

    // Load and create a texture
    GLuint planetTexture;
    glGenTextures(1, &planetTexture);
    glBindTexture(GL_TEXTURE_2D,
                  planetTexture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT);    // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    image = SOIL_load_image("textures/planets.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentally mess up our texture.

    // FPS-related variables (Frame per second)
    const double FPS = 1.0/30.0;
    register double startTime = glfwGetTime(), currentTime = 0;
    // SRR-related variables (Screen refresh rate)
    const double SRR = 2.0;
    register double endTime = 0.0, deltaTime = 0.0;
    register int frameCount = 0;

    float starMove = 0;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Update time delay according to FPS
        do
        {
            currentTime = glfwGetTime();
        } while (currentTime - startTime < FPS);
        startTime = currentTime;

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        // Render
        // Clear the colourful
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        GLint translationLocation = glGetUniformLocation(shaderProgram, "translation");
        GLint scaleLocation = glGetUniformLocation(shaderProgram, "scale");
        GLint angleLocation = glGetUniformLocation(shaderProgram, "angle");

        // Bind Texture
        glBindTexture(GL_TEXTURE_2D, planetTexture);

        glUniform3f(translationLocation, 0.0f, 0.0f, 0.0f);
        glUniform3f(scaleLocation, 1.0f, 1.0f, 1.0f);
        glUniform1f(angleLocation, 0.0f*M_PI);

        glBindVertexArray(planetVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);



        // Bind Texture
        glBindTexture(GL_TEXTURE_2D, starTexture);

        glUniform3f(translationLocation, 0.6f, -0.3f, 0.0f);
        glUniform3f(scaleLocation, 0.3f, 0.3f, 1.0f);
        glUniform1f(angleLocation, 0.25f*M_PI);

        glBindVertexArray(starVAO);
        glDrawElements(GL_TRIANGLE_FAN, sizeof(starIndices), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        starMove += 0.001f;
        glUniform3f(translationLocation, 0.8f - starMove, 0.7f, 0.0f);
        glUniform3f(scaleLocation, 0.1f, 0.1f, 1.0f);
        glUniform1f(angleLocation, 0.6f*M_PI);


        glBindVertexArray(starVAO);
        glDrawElements(GL_TRIANGLE_FAN, sizeof(starIndices), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);

        // Message update time delay according to SRR
        frameCount++;
        deltaTime += glfwGetTime() - startTime;
        if (startTime - endTime > SRR)
        {
            printf("setFPS: %5.2f \trealFPS: %5.2f \tframeTime: %5.8f\n",
                   1.0/FPS, frameCount/(startTime - endTime), deltaTime/frameCount);
            endTime = startTime;
            deltaTime = 0;
            frameCount = 0;
        }



    }

    // Properly de-allocate all resources once they've outlived their purpose
    // 1. VAO, VBO and IBO
    glDeleteVertexArrays(1, &starVAO);
    glDeleteBuffers(1, &starVBO);
    glDeleteBuffers(1, &starIBO);
    glDeleteVertexArrays(1, &planetVAO);
    glDeleteBuffers(1, &planetVBO);
    // 2. Textures
    glDeleteTextures(1, &planetTexture);
    glDeleteTextures(1, &starTexture);
    // 3. Shader Program
    glDeleteProgram(shaderProgram);
    // 4. Terminate GLFW, clearing any resources allocated by GLFW.
    glfwDestroyWindow(window);
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

//Параметры, с которыми производились расчеты, лежат в заголовочном файле
//нужно возращать ошибку!
GLchar *
glShaderLoad(char *path, int *length)
{
    GLchar *shaderSource = NULL;
    int descriptor = open(path, O_RDONLY);
    if (descriptor != -1)
    {
        FILE *file = fdopen(descriptor, "rb");
        if (file)
        {
            struct stat statistics;
            if (fstat(descriptor, &statistics) != -1)
            {
                shaderSource = (GLchar *) malloc(statistics.st_size);
                fread(shaderSource, sizeof(char), statistics.st_size, file);
                *length = (statistics.st_size);
            }
            fclose(file);
        }
        close(descriptor);
    }
//    printf("%s\n", path);
//    printf("%s\n", shaderSource);
    return shaderSource;
}


void
getShader(GLuint *shader, char *path, int typeShader)
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
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
    }
}

void
getShaderProgram(GLuint *shaderProgram, int n, ...)
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
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
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