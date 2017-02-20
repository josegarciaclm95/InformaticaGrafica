/* helloworld.cpp
 *
 * Computer Graphics / Informatica Grafica.
 * Spring 2017.
 *
 * Practice 1.
 * Introduction to OpenGL and GLUT.
 *
 * Jose Pascual Molina Masso.
 * Escuela Superior de Ingenieria Informatica de Albacete.
 */

/****************/
/* Header files */
/****************/

/* General */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Microsoft Visual C++ */
#include <windows.h>

/* OpenGL */
#include <GL/gl.h>    /* OpenGL (window system independent) */
#include <GL/glu.h>   /* OpenGL Utility library */
#include "GL/glut.h"  /* OpenGL Utility Toolkit */


/********************/
/* Global variables */
/********************/

/* Window size */
int width;   // Width
int height;  // Height

/* Store pointer coordinates and
   the state of mouse left button */
int mouseX = 0, mouseY = 0;
bool isLeftButtonPressed = false;

/* Store horizontal displacement of text */
float helloX = 0.0;
float helloY = 0.0;

bool x_movement = true;
bool y_movement = true;

int last = 0;
double distance = 0.1;
/* Array of characters */
char buffer[256];


/***********************/
/* Function prototypes */
/***********************/

void init(void);
void output(GLfloat x, GLfloat y, char* text);
void redraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void passive(int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle(void);
void idle_vert(void);
void idle_verhor(void);
void idle_bouncer(void);
/* ... to do ... */
void modifyBackColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void modifyTextColor(GLfloat red, GLfloat green, GLfloat blue);
void PopupMenu(int value);
void windowSelect(int value);
void changeAnimation(int value);
void changeTextColor(int value);
void changeBackgroundColor(int value);
void changeSpeed(int value);
//*******************************************************************

void
init(void)
{  
  /* Set black as background colour, RGB(0,0,0) */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* Set white as foreground colour, RGB(1,1,1) */
  glColor3f(1.0, 1.0, 1.0);
}

//*******************************************************************

void
output(GLfloat x, GLfloat y, char* text)
{
  char *p;

  glRasterPos2f(x, y);  
  for (p = text; *p; p++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}

//*******************************************************************

void
redraw(void)
{
  /* Clear frame buffer bits */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw 2D text */
  sprintf_s(buffer, "HELLO WORLD!!!");
  output(helloX, helloY, buffer);
  
  sprintf_s(buffer, "Pointer coordinates relative to...");
  output(10, 60, buffer);
  sprintf_s(buffer, "... the window: x= %d, y= %d", mouseX, mouseY);
  output(10, 45, buffer);
  sprintf_s(buffer, "... OpenGL    : x= %d, y= %d", mouseX, height-mouseY-1);
  output(10, 30, buffer);
  
  if (isLeftButtonPressed)
    sprintf_s(buffer, "Mouse left button: PRESSED");
  else
    sprintf_s(buffer, "Mouse left button: RELEASED");
  output(10, 15, buffer);

  /* Swap back and front buffers */
  glutSwapBuffers();
}

//*******************************************************************

void
reshape(int w, int h)
{
  /* All window space will be used as drawing area */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  /* A rectangular area is defined so that each
     area unit matches one pixel of the window */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, (GLdouble) w-1, 0, (GLdouble) h-1);
  glMatrixMode(GL_MODELVIEW);
  
  /* Store current window size */
  width = w;
  height = h;
}

//*******************************************************************

void 
keyboard(unsigned char key, int x, int y)
{
  if (key == 27)  /* Esc key */
    exit(0);
}

//*******************************************************************

void
passive(int x, int y)
{
	mouseX = x;
	mouseY = y;
	glutPostRedisplay();
}

//*******************************************************************

void
mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON) {  // If the mouse left button...
    if (state == GLUT_DOWN) {        // ...has been pressed
      isLeftButtonPressed = true;
    }
    else {
      if (state == GLUT_UP) {        // ...has been released
        isLeftButtonPressed = false;
      }
    }
    mouseX = x;
    mouseY = y;
    glutPostRedisplay();
  }
}

//*******************************************************************

void
motion(int x, int y)
{
  if (isLeftButtonPressed) {  // If the mouse left button is pressed
    mouseX = x;
    mouseY = y;
    glutPostRedisplay();
  }
}

//*******************************************************************

void 
idle(void)
{
	helloX += distance;
	if (helloX >= width) helloX = 0.0;
	glutPostRedisplay();
}

void
idle_vert(void)
{
	helloY += distance;
	if (helloY >= height) helloY = 0.0;
	glutPostRedisplay();
}

void 
idle_verhor(void)
{
	helloX += distance;
	helloY += distance;
	if (helloX >= width) helloX = 0.0;
	if (helloY >= height) helloY = 0.0;
	glutPostRedisplay();
}

void
idle_bouncer(void)
{
	//Bounce in x-axis
	if (x_movement) {
		helloX += distance;
		if (helloX >= width) {
			x_movement = false;
		}
	}
	else {
		helloX -= distance;
		if (helloX <= 0) {
			x_movement = true;
		}
	}
	//Bounce in y-axis
	if (y_movement) {
		helloY += distance;
		if (helloY >= height) {
			y_movement = false;
		}
	}
	else {
		helloY -= distance;
		if (helloY <= 0) {
			y_movement = true;
		}
	}
	glutPostRedisplay();
}
//*******************************************************************

/* ... to do ... */

//*******************************************************************

void
main(int argc, char* argv[])
{
	/* Ids for the submenus of the pop-up menu */
	int backColor;
	int textColor;
	int animation;
	int speed;
	int windowMenu;

	/* Process the command line and negotiates the start
		of an OpenGL session with the window system */
	glutInit(&argc, argv);
  
	/* Set the OpenGL display mode for the window that will be
		created next: doble buffer and RGBA colour model */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	/* Create a window for OpenGL rendering: selects the appropriate visual
		and colourmap, and create an OpenGL context associated to this window */
	glutInitWindowSize(640, 480);
	glutCreateWindow("helloworld"); 

	/* Initialize the OpenGL context */
	init();

	/* Create a pop-up menu with several submenus */
	//Menu for changing background colour
	backColor = glutCreateMenu(changeBackgroundColor);
	glutAddMenuEntry("Black", 1);
	glutAddMenuEntry("Red", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Cian", 6);
	glutAddMenuEntry("Magenta", 7);
	glutAddMenuEntry("White", 8);

	//Menu for changing text colour
	textColor = glutCreateMenu(changeTextColor);
	glutAddMenuEntry("Black", 1);
	glutAddMenuEntry("Red", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Cian", 6);
	glutAddMenuEntry("Magenta", 7);
	glutAddMenuEntry("White", 8);

	//Menu for stoping/resuming animation
	animation = glutCreateMenu(changeAnimation);
	glutAddMenuEntry("Horizontal", 1);
	glutAddMenuEntry("Vertical", 2);
	glutAddMenuEntry("Horizontal/Vertical", 3);
	glutAddMenuEntry("Bounce", 4);
	glutAddMenuEntry("Stop", 5);
	glutAddMenuEntry("Resume", 6);

	speed = glutCreateMenu(changeSpeed);
	glutAddMenuEntry("x1", 1);
	glutAddMenuEntry("x5", 5);
	glutAddMenuEntry("x10", 10);
	glutAddMenuEntry("x25", 25);

	windowMenu = glutCreateMenu(windowSelect);
	glutAddMenuEntry("Full Screen", 1);
	glutAddMenuEntry("Restore", 2);

	glutCreateMenu(PopupMenu);
	glutAddSubMenu("Animation", animation);
	glutAddSubMenu("Background", backColor);
	glutAddSubMenu("Text", textColor);
	glutAddSubMenu("Speed", speed);
	glutAddSubMenu("Window", windowMenu);
	glutAddMenuEntry("Exit", 666);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	helloX = 320.0f;

	/* Register event handler functions */
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(passive);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);

	/* Event loop */
	glutMainLoop();
}

void
changeBackgroundColor(int value) 
{
	if (value == 1) modifyBackColor(0.0, 0.0, 0.0, 0.0);
	if (value == 2) modifyBackColor(1.0, 0.0, 0.0, 0.0);
	if (value == 3) modifyBackColor(0.0, 1.0, 0.0, 0.0);
	if (value == 4) modifyBackColor(0.0, 0.0, 1.0, 0.0);
	if (value == 5) modifyBackColor(1.0, 1.0, 0.0, 0.0);
	if (value == 6) modifyBackColor(0.0, 1.0, 1.0, 0.0);
	if (value == 7) modifyBackColor(1.0, 0.0, 1.0, 0.0);
	if (value == 8) modifyBackColor(1.0, 1.0, 1.0, 0.0);
	glutPostRedisplay();
}

void
changeTextColor(int value)
{
	if (value == 1) modifyTextColor(0.0, 0.0, 0.0);
	if (value == 2) modifyTextColor(1.0, 0.0, 0.0);
	if (value == 3) modifyTextColor(0.0, 1.0, 0.0);
	if (value == 4) modifyTextColor(0.0, 0.0, 1.0);
	if (value == 5) modifyTextColor(1.0, 1.0, 0.0);
	if (value == 6) modifyTextColor(0.0, 1.0, 1.0);
	if (value == 7) modifyTextColor(1.0, 0.0, 1.0);
	if (value == 8) modifyTextColor(1.0, 1.0, 1.0);
	glutPostRedisplay();
}

void 
changeAnimation(int value)
{
	if (value == 1) glutIdleFunc(idle);
	if (value == 2) glutIdleFunc(idle_vert);
	if (value == 3) glutIdleFunc(idle_verhor);
	if (value == 4) glutIdleFunc(idle_bouncer);
	if (value == 5) glutIdleFunc(NULL);
	if (value == 6) changeAnimation(last);
	
	if (value != 5 && value != 6) last = value;
	glutPostRedisplay();
}

void
windowSelect(int value) 
{
	if (value == 1) glutFullScreen();
	if (value == 2) glutReshapeWindow(640, 480);
	glutPostRedisplay();
}

void 
changeSpeed(int value)
{
	distance = (double) value / 10;
}

void
PopupMenu(int value)
{
	if (value == 666) exit(0);
}

void 
modifyTextColor(GLfloat red, GLfloat green, GLfloat blue)
{
	glColor3f(red, green, blue);
}

void
modifyBackColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	glClearColor(red, green, blue, alpha);
}