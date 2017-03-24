/* lookat.cpp
 *
 * Informatica Grafica / Computer Graphics.
 * Spring 2017.
 *
 * Practice 4.
 * Three-dimensional viewing in OpenGL.
 *
 * Jose Pascual Molina Masso.
 * Escuela Superior de Ingenieria Informatica de Albacete.
 */

/****************/
/* Header files */
/****************/

/* General */
#include <iostream> 
#include <stdio.h>
#include <stdlib.h>

/* Microsoft Visual C++ */
#include <windows.h>

/* OpenGL */
#include "GL/glew.h"  /* OpenGL Extension Wrangler */
#include "GL/glut.h"  /* OpenGL Utility Toolkit */

/* GLM */
#include "glm/vec3.hpp" // glm::vec3
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::lookAt, glm::perspective

#include "matrices.h"


/*******************/
/* Data structures */
/*******************/

/* Window size */
int width;   // Width
int height;  // Height

/* View menu options */
bool viewAxes = true, viewHUD = true;

/* Viewpoint menu options */
enum whichPOV_type { FRONT, SIDE, TOP, BOTTOM, OTHER } whichPOV; 

/* Navigation mode ZOOM */
float zoom = 0.0;   // Object zoom
float speed = 1.0;  // Navigation speed

/* Store pointer coordinates and
   the state of mouse left button */
int lastX = 0, lastY = 0;
bool isLeftButtonPressed = false;

/* Array of characters */
char buffer[256];


/***********************/
/* Function prototypes */
/***********************/

void init(void);
void output(GLfloat x, GLfloat y, char* text);
void drawAxes(GLfloat length);
void drawModel(void);
void drawHUD(void);
void redraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void viewSelect(int value);
void drawmodeSelect(int value);
void pointsizeSelect(int value);
void linewidthSelect(int value);
void povSelect(int value);
void speedSelect(int value);
void windowSelect(int value);
void popupmenu(int value);


//*******************************************************************

void
init(void)
{  
  /* Set black as background colour, RGB(0,0,0) */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* Enable depth buffering */
  glEnable(GL_DEPTH_TEST);

  /* Set modelview matrix as the active one */
  glMatrixMode(GL_MODELVIEW);

  /* Draw triangles as lines, user can change this in the menu */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  whichPOV = FRONT;
}

//*******************************************************************

void
output(GLfloat x, GLfloat y, char* text)
{
  char *p;

  glRasterPos2f(x, y);  
  for (p = text; *p; p++)
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p);
}

//*******************************************************************

void
drawAxes(GLfloat length)
{
  /* Draw three reference axes */
  //glLineWidth(1.0);
  glBegin(GL_LINES);
	
  /* X axis, red */
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(length, 0.0, 0.0);

  /* Y axis, green */
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, length, 0.0);

  /* Z axis, blue */
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, length);
  glEnd(); 
}

//*******************************************************************

void
drawModel()
{
	glColor3f(1.0, 1.0, 1.0);

	/* Draw the Utah teapot */
	glutSolidTeapot(1.0);
}

//*******************************************************************

void
drawHUD()
{
  /* Set projection and modelview matrices to draw 2D text */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width-1, 0, height-1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
    
  glColor3f(1.0, 1.0, 1.0);  // White

  sprintf_s(buffer, "Left button and drag: ZOOM");
  output(10, 30, buffer);
  sprintf_s(buffer, "Keys + and - to zoom in-out, 0 to restore");
  output(10, 15, buffer);

  /* Restore previous projection and modelview matrices */
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

//*******************************************************************

void
redraw(void)
{
	glm::mat4 zoomMat, lookatMat, modelviewMat;

	/* Clear frame and depth buffer bits */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Navigation transformations */
	//zoomMat = glm::translate(zoomMat, glm::vec3(0.0, 0.0, -zoom));
	zoomMat = Matrices::translate(0.0, 0.0, -zoom);

	/* Set viewpoint. You can translate and rotate
	  to place the camera (moving the objects),
	  or use the lookAt function */
  
	switch (whichPOV) {
		case FRONT:
			lookatMat = Matrices::lookAt(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//lookatMat = glm::lookAt(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case SIDE:
			lookatMat = Matrices::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//lookatMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case TOP:
			lookatMat = Matrices::lookAt(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//lookatMat = glm::lookAt(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case BOTTOM:
			lookatMat = Matrices::lookAt(glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//lookatMat = glm::lookAt(glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case OTHER:
			lookatMat = Matrices::lookAt(glm::vec3(4.0f, -6.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//lookatMat = glm::lookAt(glm::vec3(4.0f, -6.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
	}
  
	modelviewMat = zoomMat * lookatMat;
	glLoadMatrixf(&modelviewMat[0][0]);

	/* Draw reference axes */
	if (viewAxes)
		drawAxes(5.0);  

	/* Draw model */
	drawModel();

	/* Draw on-screen information */
	if (viewHUD)
		drawHUD();

	/* Swap back and front buffers */
	glutSwapBuffers();
}

//*******************************************************************

void
reshape(int w, int h)
{
	glm::mat4 projectionMat;

	/* Viewport is same size as the application window */
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);

	/* Set the projection matrix as the active one */
	glMatrixMode(GL_PROJECTION);

	/* Set perspective projection parameters. Use the
	  perspective function from the GLM library */
	
	projectionMat = glm::perspective(glm::radians(40.0f), (float)w/h, 0.1f, 10000.0f);

	glLoadMatrixf(&projectionMat[0][0]);

	/* Restore modelview matrix as the active one */
	glMatrixMode(GL_MODELVIEW);

	/* Save current window size */
	width = w;
	height = h;
}

//*******************************************************************

void 
keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case '+':
      zoom -= 0.1 * speed;    // Zoom-in
      break;
    case '-':
      zoom += 0.1 * speed;    // Zoom-out
      break;
	case '0':
	  zoom = 0.0;             // Restore initial values
	  break;
    case 27:                  // Escape key
      exit(0);
  }
  glutPostRedisplay();
}

//*******************************************************************

void
mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON) {  // If the left button...
    if (state == GLUT_DOWN) {        // ...is pressed
      isLeftButtonPressed = true;
      lastX = x;
      lastY = y;
    }
    else {
      if (state == GLUT_UP) {        // ...is released
        isLeftButtonPressed = false;
	    zoom += (y - lastY)*speed/10.0;
        glutPostRedisplay();
      }
    }
  }
}

//*******************************************************************

void
motion(int x, int y)
{
  if (isLeftButtonPressed) {     // If the left button is pressed
	zoom += (y - lastY)*speed/10.0;
    lastX = x;
    lastY = y;
    glutPostRedisplay();
  }
}

//*******************************************************************

void
viewSelect(int value)
{
  if (value == 1) viewAxes = !viewAxes;
  if (value == 2) viewHUD = !viewHUD;
  glutPostRedisplay();
}

//*******************************************************************

void
drawmodeSelect(int value)
{
	switch (value) {
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 3:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glutPostRedisplay();
}

//*******************************************************************

void
pointsizeSelect(int value)
{
	switch (value) {
	case 1:
		glPointSize(1.0f);  // Default
		break;
	case 2:
		glPointSize(3.0f);
		break;
	case 3:
		glPointSize(5.0f);
	}
	glutPostRedisplay();
}

//*******************************************************************

void
linewidthSelect(int value)
{
	switch (value) {
	case 1:
		glLineWidth(1.0f);  // Default
		break;
	case 2:
		glLineWidth(3.0f);
		break;
	case 3:
		glLineWidth(5.0f);
	}
	glutPostRedisplay();
}

//*******************************************************************

void
povSelect(int value)
{
  switch (value) {
    case 1:
      whichPOV = FRONT;
	  break;
	case 2:
      whichPOV = SIDE;
	  break;
	case 3:
      whichPOV = TOP;
	  break;
	case 4:
      whichPOV = BOTTOM;
	  break;
	case 5:
      whichPOV = OTHER;
  }
  zoom = 0.0;      // Restore initial values
  glutPostRedisplay();
}

//*******************************************************************

void
speedSelect(int value)
{
  switch (value) {
    case 1:
      speed = 0.1;   // Slow navigation
	  break;
	case 2:
	  speed = 1.0;   // Normal
	  break;
	case 3:
	  speed = 10.0;  // Fast
  }
}

//*******************************************************************

void
windowSelect(int value)
{
  if (value == 1) glutFullScreen();
  if (value == 2) glutReshapeWindow(640, 480);
  glutPostRedisplay();
}

//*******************************************************************

void
popupmenu(int value)
{
  if (value == 666)
    exit(0);
}

//*******************************************************************

void
main(int argc, char* argv[])
{
  /* Ids for the submenus of the pop-up menu */
  int viewMenu, drawmodeMenu, pointsizeMenu, linewidthMenu, 
	  povMenu, speedMenu, windowMenu;

  /* Process the command line and negotiates the start
	 of an OpenGL session with the window system */
  glutInit(&argc, argv);
  
  /* Set the OpenGL display mode for the window that will be
     created next: doble buffer, RGBA colour model and depth buffer */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

  /* Create a window for OpenGL rendering: selects the appropriate visual
     and colourmap, and create an OpenGL context associated to this window */
  glutInitWindowSize(640, 480);
  glutCreateWindow("lookat");

  /* Initialize GLEW */
  GLenum err = glewInit();
  if (GLEW_OK != err) {
	  std::cerr << "Error: " << glewGetString(err) << std::endl;
  }

  /* Initialize the OpenGL context */
  init();

  /* Create a pop-up menu with several submenus */

  viewMenu = glutCreateMenu(viewSelect);
  glutAddMenuEntry("Axes", 1);
  glutAddMenuEntry("Info", 2);

  drawmodeMenu = glutCreateMenu(drawmodeSelect);
  glutAddMenuEntry("Points", 1);
  glutAddMenuEntry("Lines", 2);
  glutAddMenuEntry("Filled", 3);

  pointsizeMenu = glutCreateMenu(pointsizeSelect);
  glutAddMenuEntry("Small", 1);
  glutAddMenuEntry("Medium", 2);
  glutAddMenuEntry("Big", 3);

  linewidthMenu = glutCreateMenu(linewidthSelect);
  glutAddMenuEntry("Thin", 1);
  glutAddMenuEntry("Medium", 2);
  glutAddMenuEntry("Thick", 3);

  povMenu = glutCreateMenu(povSelect);
  glutAddMenuEntry("Front", 1);
  glutAddMenuEntry("Side", 2);
  glutAddMenuEntry("Top", 3);
  glutAddMenuEntry("Bottom", 4);
  glutAddMenuEntry("Other", 5);

  speedMenu = glutCreateMenu(speedSelect);
  glutAddMenuEntry("Slow", 1);
  glutAddMenuEntry("Normal", 2);
  glutAddMenuEntry("Fast", 3);

  windowMenu = glutCreateMenu(windowSelect);
  glutAddMenuEntry("Full screen", 1);
  glutAddMenuEntry("Restore", 2);

  glutCreateMenu(popupmenu);
  glutAddSubMenu("View", viewMenu);
  glutAddSubMenu("Draw mode", drawmodeMenu);
  glutAddSubMenu("Point size", pointsizeMenu);
  glutAddSubMenu("Line width", linewidthMenu);
  glutAddSubMenu("Point of view", povMenu);
  glutAddSubMenu("Speed", speedMenu);
  glutAddSubMenu("Window", windowMenu);
  glutAddMenuEntry("Exit", 666);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* Register event handler functions */

  /* Called each time the window needs to be redisplayed */ 
  glutDisplayFunc(redraw);

  /* Called each time the window is moved or resized */
  glutReshapeFunc(reshape);

  /* Called when a key that generates an ASCII code is pressed */
  glutKeyboardFunc(keyboard);
  
  /* Called when a mouse button is pressed or released */
  glutMouseFunc(mouse);

  /* Called when the pointer is moved within window limits
     while one or more mouse buttons are pressed */ 
  glutMotionFunc(motion);

  /* Event loop */
  glutMainLoop();
}
