/* copyright 2009 Anogeianakis Vaptistis.
 * Permition is hereby granted to compile this file and
 * execute the resulting program.
 * All other rights are reserved.
 * This software comes with no warranty.
 */

#include <gl/glew.h>
#include <gl/glut.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <windows.h>

#define width 800
#define height 800
#define THREADS 10
#define MAXITER 100
#define THRESHOLD 4
#define MAX_X 0.5
#define MIN_X -1.9
#define MAX_Y 1.2
#define MIN_Y -1.2

GLfloat picture[height][width][3];

// δεν έχει ελεγθει (αυστηρά) για ορθότητα... (δεν προλάβαινα)
// π.χ. μπορεί να γράψουν δυο διαφορετικά νήματα την ίδια σειρά
// ή μια σειρά να μην γραφεί από κανένα;
// πάντως δείχνει σωστό...
float calculateFactor(float px,float py)
{
	int c;
	float x = -0.0;
	float y = 0.0;
	float temp;
	for(c = 0 ; c < MAXITER ; ++c)
	{
		temp = x*x-y*y+px;
		y = 2.0*x*y+py;
		x = temp;
		if (x*x+y*y>THRESHOLD)
			break;
	}
	return (float)c / MAXITER;
} // end function calculateFactor


DWORD calculateRows(int i)
{
	int rows = height / THREADS;
	int remainder = height % THREADS;
	int begin = i*rows + std::min(i,remainder);
	int end = begin + rows + ((i<remainder)?1:0);
	float factor;
	for(int r = begin ; r < end ; ++r)
	{
		for(int c = 0 ; c < width ; ++c)
		{
			//factor = 0.5*sin(0.1*sqrt((float)(r-height/2)*(r-height/2)+(c-width/2)*(c-width/2))) + 0.5;
			//factor = 0.5*sin(0.05*r)*cos(0.05*c) + 0.5;
			factor = calculateFactor(c*((MAX_X-MIN_X)/width)+MIN_X,r*((MAX_Y-MIN_Y)/height)+MIN_Y);
			picture[r][c][0] = factor;
			picture[r][c][1] = factor;
			picture[r][c][2] = 0.5;
		} // end inner for
		Sleep(5);
	} // end for
	return i;
} // end function calculateRow


void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
		for(int r = 0 ; r < height ; ++r)
			for(int c = 0 ; c < width ; ++c)
			{
				glColor3fv(picture[r][c]);
				glVertex2i(c,height-1-r);
			} // end for
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
} // end function display


void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:	// escape key
		exit(0);
	} // end switch
} // end function keyboard


void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
} // end function reshape


int main(int argc, char **argv)
{
	// glut initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width,height);
	glutInitWindowPosition(1270-width,160);
	glutCreateWindow("Mandelbrot Set");

	// glew initialization
	glewInit();

	// OpenGL initialization

	// thread creation
	HANDLE threadHandle;
	DWORD threadID;
	for(int t = 0 ; t < THREADS ; ++t)
	{
		threadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)calculateRows,(void*)t,0,&threadID);
		CloseHandle(threadHandle);
	} // end for

	// event handling initialization
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
} // end function main
