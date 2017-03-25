/* stereo.cpp
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

/* Descripcion de una camara estereo */
GLfloat dist;    // Distancia del observador a la pantalla
GLfloat eyesep;  // Eye separation, interpupillary or interocular distance

/* Navigation mode ORBIT */
float orbitX = 0.0, 
      orbitY = 0.0; // Rotation angles, in degrees,
					//   around X and Y axes
float speed = 1.0;  // Navigation speed

/* Store pointer coordinates and
   the state of mouse left button */
int lastX = 0, lastY = 0;
bool isLeftButtonPressed = false;

/* Stereo menu options */
enum stereoMode_type { ANAGLYPH, INTERLACED1, INTERLACED2 } stereoMode;

/* Array of characters */
char buffer[256];


/***********************/
/* Function prototypes */
/***********************/

void init(void);
void stereoEnable(void);
void stereoDisable(void);
void output(GLfloat x, GLfloat y, char* text);
void drawStencilPattern(void);
void drawAxes(GLfloat length);
void drawModel(void);
void drawHUD(void);
void redraw(void);
void stereoPreImg(void);
void stereoPreLeftImg(void);
void stereoInterLeftRightImg(void);
void stereoPreRightImg(void);
void stereoPostImg(void);
void stereoRedraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void viewSelect(int value);
void drawmodeSelect(int value);
void pointsizeSelect(int value);
void linewidthSelect(int value);
void bgcolorSelect(int value);
void stereoSelect(int value);
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

  dist = 50.0;
  eyesep = dist / 15.0;  // Recommended
  stereoMode = ANAGLYPH;
}

//*******************************************************************

void
stereoEnable(void)
{
  if (stereoMode == INTERLACED1 || stereoMode == INTERLACED2) {
    glClearStencil(0x0);
    glEnable(GL_STENCIL_TEST);  /* Enable stenciling */
  }
}

//*******************************************************************

void
stereoDisable(void)
{
  if (stereoMode == INTERLACED1 || stereoMode == INTERLACED2)
    glDisable(GL_STENCIL_TEST);  /* Disable stenciling */
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
drawStencilPattern(void)
{
  /* Set projection and modelview matrices to draw 2D pattern */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width-1, 0, height-1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
    
  glDisable(GL_DEPTH_TEST);

  glClear(GL_STENCIL_BUFFER_BIT);
  glStencilFunc (GL_ALWAYS, 0x1, 0x1);
  glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);

  /* Draw pattern */
  glColor3f(1.0, 1.0, 1.0);
  for (int y=height-1; y>=0; y-=2)
  {
    glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2i(0, y);
	glVertex2i(width, y);
	glEnd();	
  }
  
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

  glEnable(GL_DEPTH_TEST);

  /* Restore previous projection and modelview matrices */
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
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
drawModel(void)
{
	glColor3f(1.0, 1.0, 1.0);

	/* Draw the Utah teapot */
	glutSolidTeapot(10.0);
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
    
  glDisable(GL_DEPTH_TEST);
  glColor3f(1.0, 1.0, 1.0);
  
  switch (stereoMode) {
	case ANAGLYPH:
      output(10, height-15, "Red filter in left eye, blue in right eye");
      break;
	case INTERLACED1:
	case INTERLACED2:
      output(10, height-15, "The Zalman monitor resolution must be 1680x1050");
  }
  output(10, height-30, "Keys W and X to change distance to screen");
  output(10, height-45, "Keys A and D to change interpupillary distance");
  sprintf_s(buffer, "Distance to screen: %.4f", dist);
  output(10, height-60, buffer);
  sprintf_s(buffer, "Interpupillary distance: %.4f", eyesep);
  output(10, height-75, buffer);

  sprintf_s(buffer, "Left button and drag: ORBIT");
  output(10, 30, buffer);
  sprintf_s(buffer, "Keys 4,8,2 and 6 to orbit, 0 to restore");
  output(10, 15, buffer);

  glEnable(GL_DEPTH_TEST);

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
	glm::mat4 zoomMat, orbitMat, viewMat;

	/* Clear frame and depth buffer bits */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Navigation transformations */
	//zoomMat = glm::translate(zoomMat, glm::vec3(0.0, 0.0, -dist));
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitX), glm::vec3(1.0, 0.0, 0.0));
	orbitMat = glm::rotate(orbitMat, glm::radians(-orbitY), glm::vec3(0.0, 1.0, 0.0));
	zoomMat = Matrices::translate(0.0, 0.0, -dist);
	orbitMat = Matrices::rotateX(-orbitX) * Matrices::rotateY(-orbitY);

	viewMat = zoomMat * orbitMat;
	glLoadMatrixf(&viewMat[0][0]);

	/* Draw reference axes */
	if (viewAxes)
		drawAxes(15.0);  

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
stereoPreImg(void)
{
  if (stereoMode == INTERLACED1 || stereoMode == INTERLACED2)
    drawStencilPattern();
}

//*******************************************************************

void
stereoPreLeftImg(void)
{
  switch (stereoMode) {
	case ANAGLYPH:
		//glClear(GL_COLOR_BUFFER_BIT);
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
		break;
	case INTERLACED1:
		glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
		break;
	case INTERLACED2:
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		break;
  }
}
//*******************************************************************

void
stereoInterLeftRightImg(void)
{
  if (stereoMode == ANAGLYPH)
      glClear(GL_DEPTH_BUFFER_BIT);  /* Clear the depth bit only */
}

//*******************************************************************

void
stereoPreRightImg(void)
{
  switch (stereoMode) {
	case ANAGLYPH:
		//glClear(GL_COLOR_BUFFER_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
		break;
	case INTERLACED1:
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		break;
	case INTERLACED2:
		glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
		break;
  }
}

//*******************************************************************

void
stereoPostImg(void)
{
  switch (stereoMode) {
	case ANAGLYPH:
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
		break;
	case INTERLACED1:
	case INTERLACED2:
		glStencilFunc (GL_ALWAYS, 0x1, 0x1);
  }
}

//*******************************************************************

void
stereoRedraw(void)
{
	glm::mat4 lookatMat, orbitMat, viewMat;

	/* Draw stencial pattern for the interleaved stereo mode */
	stereoPreImg();

	/* Clear frame and depth buffer bits */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Left eye, red filter */
	stereoPreLeftImg();

	/* Set left eye viewpoint, store the matrix in lookatMat */
	lookatMat = Matrices::lookAt(glm::vec3(-eyesep/2, 0.0f, dist), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	/* Navigation transformations */
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitX), glm::vec3(1.0, 0.0, 0.0));
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitY), glm::vec3(0.0, 1.0, 0.0));
	orbitMat = Matrices::rotateX(-orbitX) * Matrices::rotateY(-orbitY);

	viewMat = lookatMat * orbitMat;
	glLoadMatrixf(&viewMat[0][0]);

	/* Draw reference axes */
	if (viewAxes)
		drawAxes(15.0);  

	/* Draw model */
	drawModel();

	/* If anaglyph mode, clear depth buffer bits */
	stereoInterLeftRightImg();

	/* Right eye, blue filter */
	stereoPreRightImg();

	/* Set right eye viewpoint, store the matrix in lookatMat */
	lookatMat = Matrices::lookAt(glm::vec3(eyesep / 2, 0.0f, dist), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	/* Navigation transformations (same as the left eye) */
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitX), glm::vec3(1.0, 0.0, 0.0));
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitY), glm::vec3(0.0, 1.0, 0.0));
	orbitMat = Matrices::rotateX(-orbitX) * Matrices::rotateY(-orbitY);

	viewMat = lookatMat * orbitMat;
	glLoadMatrixf(&viewMat[0][0]);

	/* Draw reference axes */
	if (viewAxes)
		drawAxes(15.0);  

	/* Draw model */
	drawModel();

	/* Masks are no longer needed */
	stereoPostImg();
  
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
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	/* Set the projection matrix as the active one */
	glMatrixMode(GL_PROJECTION);

	/* Set perspective projection parameters. Use the
	  perspective function from the GLM library */

	projectionMat = glm::perspective(glm::radians(40.0f), (float)w / h, 0.1f, 10000.0f);

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
    case 'A':
    case 'a':
      eyesep -= 0.01;  // Decrease interpupillary distance
      break;
    case 'D':
    case 'd':
      eyesep += 0.01;  // Increase interpupillary distance
      break;
    case 'W':
    case 'w':
      dist -= 0.1;     // Decrease distance to screen
      break;
    case 'X':
    case 'x':
      dist += 0.1;     // Increase distance to screen
      break;
    case '4':
      orbitY += 1.0 * speed;  // Orbit to the right
      break;
    case '6':
      orbitY -= 1.0 * speed;  // Orbit to the left
      break;
    case '8':
      orbitX += 1.0 * speed;  // Orbit downwards
      break;
    case '2':
      orbitX -= 1.0 * speed;  // Orbit upwards
      break;
	case '0':
	  orbitX = 0.0;  // Restore initial values
	  orbitY = 0.0;
	  break;
    case 27:         // Escape key
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
        orbitX += (lastY - y)*speed;
        orbitY += (lastX - x)*speed;
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
    orbitX += (lastY - y)*speed;
    orbitY += (lastX - x)*speed;
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
bgcolorSelect(int value)
{
  switch (value) {
    case 1: 
	  glClearColor(0.0, 0.0, 0.0, 0.0);  // Black 
	  break;
    case 2: 
	  glClearColor(0.3, 0.3, 0.3, 0.0);  // Dark grey 
	  break;
    case 3: 
	  glClearColor(0.5, 0.5, 0.5, 0.0);  // Grey 
	  break;
    case 4: 
	  glClearColor(0.7, 0.7, 0.7, 0.0);  // Bright grey 
	  break;
    case 5: 
	  glClearColor(1.0, 0.0, 0.0, 0.0);  // Red 
	  break;
    case 6: 
	  glClearColor(0.0, 1.0, 0.0, 0.0);  // Green
	  break;
    case 7: 
	  glClearColor(0.0, 0.0, 1.0, 0.0);  // Blue
	  break;
    case 8: 
	  glClearColor(1.0, 1.0, 0.0, 0.0);  // Yellow
	  break;
    case 9: 
	  glClearColor(0.0, 1.0, 1.0, 0.0);  // Cyan
	  break;
    case 10: 
	  glClearColor(1.0, 0.0, 1.0, 0.0);  // Magenta
	  break;
    case 11: 
	  glClearColor(1.0, 1.0, 1.0, 0.0);  // White 
  }

  glutPostRedisplay();
}

//*******************************************************************

void
stereoSelect(int value)
{
  if (value == 1) {
      stereoDisable();
      glutDisplayFunc(redraw);
  }
  else {
      if (value == 2) stereoMode = ANAGLYPH;
      if (value == 3) stereoMode = INTERLACED1;
      if (value == 4) stereoMode = INTERLACED2;
      stereoEnable();
      glutDisplayFunc(stereoRedraw);
  }
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
  if (value == 2) glutReshapeWindow(1024, 768);
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
	  bgcolorMenu, stereoMenu, speedMenu, windowMenu;

  /* Process the command line and negotiates the start
	 of an OpenGL session with the window system */
  glutInit(&argc, argv);
  
  /* Set the OpenGL display mode for the window that will be
     created next: doble buffer, RGBA colour model, depth buffer and stencil buffer */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);

  /* Create a window for OpenGL rendering: selects the appropriate visual
     and colourmap, and create an OpenGL context associated to this window */
  glutInitWindowSize(1024, 768);
  glutCreateWindow("stereo");

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

  bgcolorMenu = glutCreateMenu(bgcolorSelect);
  glutAddMenuEntry("Black", 1);
  glutAddMenuEntry("Dark grey", 2);
  glutAddMenuEntry("Grey", 3);
  glutAddMenuEntry("Bright grey", 4);
  glutAddMenuEntry("Red", 5);
  glutAddMenuEntry("Green", 6);
  glutAddMenuEntry("Blue", 7);
  glutAddMenuEntry("Yellow", 8);
  glutAddMenuEntry("Cyan", 9);
  glutAddMenuEntry("Magenta", 10);
  glutAddMenuEntry("White", 11);

  stereoMenu = glutCreateMenu(stereoSelect);
  glutAddMenuEntry("Disable", 1);
  glutAddMenuEntry("Red-blue anaglyph", 2);
  glutAddMenuEntry("Interlaced 1", 3);
  glutAddMenuEntry("Interlaced 2", 4);

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
  glutAddSubMenu("Background", bgcolorMenu);
  glutAddSubMenu("Stereo", stereoMenu);
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
