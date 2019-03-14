//Daninthia Fox

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <ctime>
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
using namespace std;

GLint SCREEN_WIDTH = 750;
GLint SCREEN_HEIGHT = 750;
GLint SCREEN_DEPTH = 750;


float myRand(float R)
{
	return( 2  *  R  * rand()) / RAND_MAX - R;
}


void fireworks(float x1, float y1, float z1)
{
	float red   = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float green = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float blue  = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	glColor3f(red, green, blue);

	for(int i = 0; i < rand() % (88 - 32  +8) + 100; ++i)
	{
		glVertex3f(x1, y1, z1);
		float x2 = myRand(66) + x1;
		float y2 = myRand(76) + y1;
		float z2 = myRand(86) + z1;
		glVertex3f(x2, y2, z2);
	} 
}

//init function


void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float radius = 100;
	glOrtho(-radius, radius, -radius, radius, -radius, radius);
}

//display
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	float x1, y1, z1;

	
	for(int j = 0; j < rand() % (25 - 9 + 3) + 4; ++j)
	{
		x1 = myRand(150);
		y1 = myRand(150);
		z1 = myRand(150);
		fireworks(x1, y1, z1);
	}
	glEnd();
	glFlush();

}


//main

int main(int argc, char *argv[])
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGB| GLUT_SINGLE| GLUT_DEPTH);
	glutCreateWindow("Fireworks :)");
	glutDisplayFunc(display);

	init();
	glutMainLoop();

	return 0;
	
}

