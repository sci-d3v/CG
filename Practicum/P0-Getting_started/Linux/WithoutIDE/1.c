//gcc 1.c -lglut -lGLU -lGL -o 1

#include <GL/glut.h> 

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void lineSegment(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(5);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
		glVertex2i(10, 10); 	
		glVertex2i(15, 15);
	glEnd();
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 300);

	glutCreateWindow("An Example OpenGL Program");
	init();
	glutDisplayFunc(lineSegment);
	glutMainLoop();
	return 0;
}
