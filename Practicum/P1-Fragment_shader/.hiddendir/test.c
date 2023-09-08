#include <GL/glut.h>

// Define the pixel shader program
const char* pixelShaderSource = "#version 120\n"
                                "void main() {\n"
                                "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
                                "}\n";

int main(int argc, char** argv) {
    // Initialize GLUT and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Pixel Shader Example");

    // Set up the OpenGL context
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    // Create the vertex buffer
    GLfloat vertices[] = {-1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0};
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create the pixel shader program
    GLuint program = glCreateProgram();
    GLuint pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pixelShader, 1, &pixelShaderSource, NULL);
    glCompileShader(pixelShader);
    glAttachShader(program, pixelShader);
    glLinkProgram(program);
    glUseProgram(program);

    // Render the scene using the pixel shader
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Clean up resources
    glDisableClientState(GL_VERTEX_ARRAY);
    glDeleteBuffers(1, &buffer);
    glDetachShader(program, pixelShader);
    glDeleteShader(pixelShader);
    glDeleteProgram(program);
    glutMainLoop();
    return 0;
}