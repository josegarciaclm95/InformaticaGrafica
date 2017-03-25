/* zbuffer.cpp
 *
 * Informatica Grafica / Computer Graphics.
 * Spring 2017.
 *
 * Practice 5.
 * Visible-surface detection in OpenGL.
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
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale

#include "matrices.h"


/*******************/
/* Data structures */
/*******************/

/* Window size */
int width;   // Width
int height;  // Height

/* View menu options */
bool viewAxes = true, viewHUD = true;

/* Visible-surface detection options */
bool depthtest = true, cullface = false, ccw = true; 

/* Navigation modes */
enum navMode_type { ZOOM, ORBIT } navMode;
				 
float dist = 5.0;  // Distance to object
float orbitX = 0.0, 
      orbitY = 0.0; // Rotation angles, in degrees,
					//   around X and Y axes
float speed = 1.0;  // Navigation speed

/* Store pointer coordinates and
   the state of mouse left button */
int lastX = 0, lastY = 0;
bool isLeftButtonPressed = false;

/* Array of characters */
char buffer[256];

/* Pyramid vertex-array and buffer objects */
GLuint pyramid_vao, pyramid_vbo;

/* Macro used in glVertexAttribPointer() calls, from the red book */
#define BUFFER_OFFSET(bytes) ((GLubyte *) NULL + (bytes))


/***********************/
/* Function prototypes */
/***********************/

void initPyramid(void);
void init(void);
void output(GLfloat x, GLfloat y, char* text);
void drawAxes(GLfloat length);
void drawPyramid(void);
void drawModel(void);
void drawHUD(void);
void redraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void viewSelect(int value);
void zbufferSelect(int value);
void frontfaceSelect(int value);
void backfaceSelect(int value);
void navmodeSelect(int value);
void windowSelect(int value);
void trimonSelect(int value);
void popupmenu(int value);


//*******************************************************************

void
initPyramid(void)
{
	// A square pyramid, statically allocated
	static const GLfloat pyramid_positions[] =
	{
		0.5f, 0.0f,  0.5f,	 // Bottom (2 triangles)
		-0.5f, 0.0f,  0.5f,
		-0.5f, 0.0f, -0.5f,
		-0.5f, 0.0f, -0.5f,
		0.5f, 0.0f, -0.5f,
		0.5f, 0.0f,  0.5f,
		-0.5f, 0.0f,  0.5f,	 // Front face
		0.5f, 0.0f,  0.5f,
		0.0f, 1.0f,  0.0f,
		0.5f, 0.0f, -0.5f,	 // Rear face
		-0.5f, 0.0f, -0.5f,
		0.0f, 1.0f,  0.0f,
		0.5f, 0.0f,  0.5f,	 // Right face
		0.5f, 0.0f, -0.5f,
		0.0f, 1.0f,  0.0f,
		-0.5f, 0.0f, -0.5f,	 // Left face
		-0.5f, 0.0f,  0.5f,
		0.0f, 1.0f,  0.0f
	};

	// Set up the vertex-array object (VAO)
	glGenVertexArrays(1, &pyramid_vao);  // Reserve a name for the object
	glBindVertexArray(pyramid_vao);  // Make the object current

									 // The vertex-related data of a VAO is stored in a vertex-buffer object (VBO) 
	glGenBuffers(1, &pyramid_vbo);  // Reserve a name for the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vbo);  // Bind it to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_positions), pyramid_positions, GL_STATIC_DRAW);
	// Allocate space for it (sizeof(positions)) and copy data

	// Connect data to vertex shader, the entrance to OpenGL pipeline
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
}

//*******************************************************************

void
init(void)
{  
  /* Set black as background colour, RGB(0,0,0) */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* Enable depth buffering */
  glEnable(GL_DEPTH_TEST);

  /* Assume that the vertices are defined in counterclockwise
     direction when looking the visible side of polygons,
     although back-face culling is initially disabled */
  glFrontFace(GL_CCW);
  glDisable(GL_CULL_FACE);

  /* Set modelview matrix as the active one */
  glMatrixMode(GL_MODELVIEW);

  /* Set up the geometry */
  initPyramid();

  /* Draw triangles as lines, user can change this in the menu */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  navMode = ORBIT;
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
drawPyramid(void)
{
	// Nonindexed drawing
	glBindVertexArray(pyramid_vao);  // Make the object current
	glDrawArrays(GL_TRIANGLES, 0, 18);  // count = 18 vertex positions
}

//*******************************************************************

void
drawModel(void)
{
	glColor3f(1.0, 1.0, 1.0);

	drawPyramid();
}

//*******************************************************************

void
drawHUD(void)
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

  if (depthtest)
    output(10, height-15, "Z-buffer method enabled");
  else
    output(10, height-15, "Z-buffer method disabled");

  if (!cullface) {
    output(10, height-30, "Back-face culling disabled");
  } else {
    output(10, height-30, "Back-face culling enabled");
    if (ccw)
      output(10, height-45, "Vertices defined in counterclockwise direction, viewed from front face");
    else
      output(10, height-45, "Vertices defined in clockwise direction, viewed from front face");
  }

  switch (navMode) {
    case ZOOM:
      sprintf_s(buffer, "Left button and drag: ZOOM");
      output(10, 30, buffer);
	  break;
	case ORBIT:
      sprintf_s(buffer, "Left button and drag: ORBIT");
      output(10, 30, buffer);
  }
  sprintf_s(buffer, "Keys 4,8,2 and 6 to orbit, + and - to zoom in-out, 0 to restore");
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
	glm::mat4 zoomMat, orbitMat, viewMat;

	/* Clear frame and depth buffer bits */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Navigation transformations */
	zoomMat = glm::translate(zoomMat, glm::vec3(0.0, 0.0, -dist));
	orbitMat = glm::rotate(orbitMat, glm::radians(-orbitX), glm::vec3(1.0, 0.0, 0.0));
	orbitMat = glm::rotate(orbitMat, glm::radians(-orbitY), glm::vec3(0.0, 1.0, 0.0));

	viewMat = zoomMat * orbitMat;
	glLoadMatrixf(&viewMat[0][0]);

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
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	/* Set the projection matrix as the active one */
	glMatrixMode(GL_PROJECTION);

	/* Set perspective projection parameters. Use the
	perspective function from the GLM library */
	projectionMat = glm::perspective(
		glm::radians(40.0f),  /* FOV, field of view (vertical), in radians */
		(float)w / h,		  /* Aspect ratio width:height */
		0.1f, 10000.0f		  /* Front face (Z near, hither) and rear face
							  (Z far, yon) of the viewing volume */
	);
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
      dist -= 0.1 * speed;    // Zoom-in
      break;
    case '-':
      dist += 0.1 * speed;    // Zoom-out
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
	  dist = 10.0;   // Restore initial values
	  orbitX = 0.0;
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
		switch (navMode) {
		  case ZOOM:
		    dist += (y - lastY)*speed/10.0;
			if (dist < 1.0) dist = 1.0;
			break;
		  case ORBIT:
            orbitX += (lastY - y)*speed;
            orbitY += (lastX - x)*speed;
		}
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
	switch (navMode) {
	  case ZOOM:
	    dist += (y - lastY)*speed/10.0;
		if (dist < 1.0) dist = 1.0;
		break;
	  case ORBIT:
        orbitX += (lastY - y)*speed;
        orbitY += (lastX - x)*speed;
	}
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
zbufferSelect(int value)
{
  if (value == 1) {
    glEnable(GL_DEPTH_TEST);
	depthtest = true;
  }
  if (value == 2) { 
    glDisable(GL_DEPTH_TEST);
	depthtest = false;
  }
  glutPostRedisplay();
}

//*******************************************************************

void
backfaceSelect(int value)
{
  if (value == 1) {
    glEnable(GL_CULL_FACE);
	cullface = true;
  }
  if (value == 2) { 
    glDisable(GL_CULL_FACE);
	cullface = false;
  }
  glutPostRedisplay();
}

//*******************************************************************

void
frontfaceSelect(int value)
{
  if (value == 1) {
    glFrontFace(GL_CCW);
	ccw = true;
  }
  if (value == 2) { 
    glFrontFace(GL_CW);
	ccw = false;
  }
  glutPostRedisplay();
}

//*******************************************************************

void
navmodeSelect(int value)
{
  switch (value) {
    case 1:
      navMode = ZOOM;   // Zoom in-out
	  break;
	case 2:
	  navMode = ORBIT;  // Orbit around the model
	  break;
	case 3:
	  dist = 10.0;      // Restore initial values
	  orbitX = 0.0;
	  orbitY = 0.0;
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
  int viewMenu, zbufferMenu, backfaceMenu, frontfaceMenu, navmodeMenu, speedMenu, windowMenu;  

  /* Process the command line and negotiates the start
	 of an OpenGL session with the window system */
  glutInit(&argc, argv);
  
  /* Set the OpenGL display mode for the window that will be created
     next: doble buffer, RGBA colour model, depth buffer and stencil buffer */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);

  /* Create a window for OpenGL rendering: selects the appropriate visual
     and colourmap, and create an OpenGL context associated to this window */
  glutInitWindowSize(640, 480);
  glutCreateWindow("zbuffer");

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

  zbufferMenu = glutCreateMenu(zbufferSelect);
  glutAddMenuEntry("Enable", 1);
  glutAddMenuEntry("Disable", 2);

  backfaceMenu = glutCreateMenu(backfaceSelect);
  glutAddMenuEntry("Enable culling", 1);
  glutAddMenuEntry("Disable culling", 2);

  frontfaceMenu = glutCreateMenu(frontfaceSelect);
  glutAddMenuEntry("Counterclockwise", 1);
  glutAddMenuEntry("Clockwise", 2);

  navmodeMenu = glutCreateMenu(navmodeSelect);
  glutAddMenuEntry("Zoom", 1);
  glutAddMenuEntry("Orbit", 2);
  glutAddMenuEntry("Restore", 3);

  speedMenu = glutCreateMenu(speedSelect);
  glutAddMenuEntry("Slow", 1);
  glutAddMenuEntry("Normal", 2);
  glutAddMenuEntry("Fast", 3);

  windowMenu = glutCreateMenu(windowSelect);
  glutAddMenuEntry("Full screen", 1);
  glutAddMenuEntry("Restore", 2);

  glutCreateMenu(popupmenu);
  glutAddSubMenu("View", viewMenu);
  glutAddSubMenu("Z-buffer", zbufferMenu);
  glutAddSubMenu("Back faces", backfaceMenu);
  glutAddSubMenu("Front faces", frontfaceMenu);
  glutAddSubMenu("Navigation", navmodeMenu);
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
