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

#include "LoadShaders.h"
#include "matrices.h"


/*******************/
/* Data structures */
/*******************/

/* Window size */
int width;   // Width
int height;  // Height

/* Show reference axes */
bool viewAxes = true;

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

/* Shader variables */
GLuint render_prog; 
GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;

/* Axes vertex-array and buffer objects */
GLuint axes_vao, axes_vbo;

/* Pyramid vertex-array and buffer objects */
GLuint pyramid_vao, pyramid_vbo, pyramid_ebo;

/* Macro used in glVertexAttribPointer() calls, from the red book */
#define BUFFER_OFFSET(bytes) ((GLubyte *) NULL + (bytes))


/***********************/
/* Function prototypes */
/***********************/

void printHelp(void);
void initAxes(void);
void initPyramid(void);
void init(void);
void drawAxes(GLfloat length);
void drawPyramid(void);
void redraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void axesSelect(int value);
void zbufferSelect(int value);
void frontfaceSelect(int value);
void backfaceSelect(int value);
void navmodeSelect(int value);
void windowSelect(int value);
void popupmenu(int value);

//*******************************************************************

void
printHelp(void)
{
	std::cout << "\nHelp:\n"
		<< "Mouse left button and drag to navigate\n"
		<< "Keys 4,8,2 and 6 to orbit, + and - to zoom in-out, 0 to restore\n"
		<< "Press h to print this help again.\n"
		<< "Press Esc to quit.\n";

	std::cout << "\nCurrent settings:\n";
	if (depthtest) 
		std::cout << "Z-buffer method enabled\n";
	else
		std::cout << "Z-buffer method disabled\n";
	if (cullface)
		std::cout << "Back-face culling enabled\n";
	else 
		std::cout << "Back-face culling disabled\n";
	if (ccw)
		std::cout << "Vertices defined in counterclockwise direction, viewed from front face\n";
	else
		std::cout << "Vertices defined in clockwise direction, viewed from front face\n";
	if (navMode == ZOOM) 
		std::cout << "Left button and drag: ZOOM\n";
	if (navMode == ORBIT)
		std::cout << "Left button and drag: ORBIT\n";
	std::cout << "\n";
}

//*******************************************************************

void
initAxes(void)
{
	// Three reference axes
	static const GLfloat axes_positions[] =
	{
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	// Color for each vertex
	static const GLfloat axes_colors[] =
	{
		1.0f, 0.0f, 0.0f,  // Red
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  // Green
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,  // Blue
		0.0f, 0.0f, 1.0f,
	};

	// Set up the vertex-array object (VAO)
	glGenVertexArrays(1, &axes_vao);  // Reserve a name for the object
	glBindVertexArray(axes_vao);  // Make the object current

	// The vertex-related data of a VAO is stored in a vertex-buffer object (VBO) 
	glGenBuffers(1, &axes_vbo);  // Reserve a name for the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, axes_vbo);  // Bind it to the GL_ARRAY_BUFFER target
	// Allocate space for it (sizeof(positions)+sizeof(colors)) and copy data
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_positions) + sizeof(axes_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axes_positions), axes_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(axes_positions), sizeof(axes_colors), axes_colors);

	// Connect data to vertex shader, the entrance to OpenGL pipeline
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(axes_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

//*******************************************************************

void
initPyramid(void)
{
	// An indexed pyramid, statically allocated
	static const GLfloat pyramid_positions[] =
	{
		0.5f, 0.0f,  0.5f,	 // Vertex 0
		-0.5f, 0.0f,  0.5f,  // 1
		-0.5f, 0.0f, -0.5f,  // 2
		0.5f, 0.0f, -0.5f,  // 3
		0.0f, 1.0f,  0.0f,  // 4, apex
	};

	// Color for each vertex
	static const GLfloat pyramid_colors[] =
	{
		1.0f, 1.0f, 0.0f,  // Yellow
		0.0f, 0.0f, 1.0f,  // Blue
		0.0f, 1.0f, 0.0f,  // Green
		1.0f, 0.0f, 0.0f,  // Red
		1.0f, 1.0f, 1.0f   // White
	};

	// Indices for the triangles
	static const GLushort pyramid_indices[] =
	{
		0, 1, 2,  // Bottom
		2, 3, 0,
		1, 0, 4,  // Front
		3, 2, 4,  // Rear
		0, 3, 4,  // Right
		2, 1, 4   // Left
	};

	// The indices are stored in an element-array buffer (EBO) 
	glGenBuffers(1, &pyramid_ebo);  // Reserve a name for the buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_ebo);  // Bind it to the GL_ELEMENT_ARRAY_BUFFER target
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_indices), pyramid_indices, GL_STATIC_DRAW);
	// Allocate space for it (sizeof(positions)) and copy data

	// Set up the vertex-array object (VAO)
	glGenVertexArrays(1, &pyramid_vao);  // Reserve a name for the object
	glBindVertexArray(pyramid_vao);  // Make the object current

	// The vertex-related data of a VAO is stored in a vertex-buffer object (VBO)
	glGenBuffers(1, &pyramid_vbo);  // Reserve a name for the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_vbo);  // Bind it to the GL_ARRAY_BUFFER target
	// Allocate space for it (sizeof(positions)+sizeof(colors)) and copy data
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_positions) + sizeof(pyramid_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pyramid_positions), pyramid_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pyramid_positions), sizeof(pyramid_colors), pyramid_colors);

	// Connect data to vertex shader, the entrance to OpenGL pipeline
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(pyramid_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

//*******************************************************************

void
init(void)
{  
  /* Set black as background colour, RGB(0,0,0) */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* Init shaders */
  ShaderInfo shader_info[] =
  {
	  { GL_VERTEX_SHADER, "./shaders/zbuffer.vert.glsl" },
	  { GL_FRAGMENT_SHADER, "./shaders/zbuffer.frag.glsl" },
	  { GL_NONE, NULL }
  };

  render_prog = LoadShaders(shader_info);

  glUseProgram(render_prog);

  render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
  render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

  /* Set up the geometry */
  initAxes();
  initPyramid();

  /* Enable depth buffering */
  glEnable(GL_DEPTH_TEST);

  /* Assume that the vertices are defined in counterclockwise
  direction when looking the visible side of polygons,
  although back-face culling is initially disabled */
  glFrontFace(GL_CCW);
  glDisable(GL_CULL_FACE);

  navMode = ORBIT;
}

//*******************************************************************

void
drawAxes(GLfloat length)
{
	// Nonindexed drawing
	glBindVertexArray(axes_vao);  // Make the object current
	glDrawArrays(GL_LINES, 0, 6);  // count = 6 vertex positions
}

//*******************************************************************

void
drawPyramid(void)
{
	// Indexed drawing

	// Set up for a glDrawElements call
	glBindVertexArray(pyramid_vao);  // Make the object current
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_ebo);

	// DrawElements
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, NULL);  // count = 18 indices 
}

//*******************************************************************

void
redraw(void)
{
	glm::mat4 zoomMat, orbitMat, viewMat, axesMat, pyramidMat;

	/* Clear frame and depth buffer bits */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Activate simple shading program */
	glUseProgram(render_prog);

	/* Navigation transformations */
	zoomMat = glm::translate(zoomMat, glm::vec3(0.0, 0.0, -dist));
	orbitMat = glm::rotate(orbitMat, glm::radians(-orbitX), glm::vec3(1.0, 0.0, 0.0));
	orbitMat = glm::rotate(orbitMat, glm::radians(-orbitY), glm::vec3(0.0, 1.0, 0.0));

	viewMat = zoomMat * orbitMat;

	/* Draw reference axes */
	if (viewAxes) {
		axesMat = glm::scale(axesMat, glm::vec3(5.0f, 5.0f, 5.0f));
		axesMat = viewMat * axesMat;
		glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, &axesMat[0][0]);

		drawAxes(5.0);
	}

	/* Draw model */
	pyramidMat = viewMat;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, &pyramidMat[0][0]);
	drawPyramid();

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

	/* Set perspective projection parameters. Use the
	perspective function from the GLM library */
	projectionMat = glm::perspective(
		glm::radians(40.0f),  /* FOV, field of view (vertical), in radians */
		(float)w / h,		  /* Aspect ratio width:height */
		0.1f, 10000.0f		  /* Front face (Z near, hither) and rear face
							  (Z far, yon) of the viewing volume */
	);

	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, &projectionMat[0][0]);

	/* Save current window size */
	width = w;
	height = h;
}

//*******************************************************************

void 
keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 'H':
    case 'h':
	  printHelp();  // Print help again
	  break;
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
axesSelect(int value)
{
  if (value == 1) viewAxes = true;
  if (value == 2) viewAxes = false;
  glutPostRedisplay();
}

//*******************************************************************

void
zbufferSelect(int value)
{
  if (value == 1) {
	if (!depthtest) {
		depthtest = true;
		glEnable(GL_DEPTH_TEST);
		std::cout << "Z-buffer method enabled\n";
	}
	else {
		std::cout << "Z-buffer method already enabled\n";
	}
  }
  if (value == 2) { 
	if (depthtest) {
		depthtest = false;
		glDisable(GL_DEPTH_TEST);
		std::cout << "Z-buffer method disabled\n";
	}
	else {
		std::cout << "Z-buffer method already disabled\n";
	}
  }
  glutPostRedisplay();
}

//*******************************************************************

void
backfaceSelect(int value)
{
	if (value == 1) {
		if (!cullface) {
			cullface = true;
			glEnable(GL_CULL_FACE);
			std::cout << "Back-face culling enabled\n";
		}
		else {
			std::cout << "Back-face culling already enabled\n";
		}
	}
	if (value == 2) {
		if (cullface) {
			cullface = false;
			glDisable(GL_CULL_FACE);
			std::cout << "Back-face culling disabled\n";
		}
		else {
			std::cout << "Back-face culling already disabled\n";
		}
	}
	glutPostRedisplay();
}

//*******************************************************************

void
frontfaceSelect(int value)
{
	if (value == 1) {
		if (!ccw) {
			ccw = true;
			glFrontFace(GL_CCW);
			std::cout << "Vertices defined in counterclockwise direction, viewed from front face\n";
		}
		else {
			std::cout << "Vertices already defined in counterclockwise direction, viewed from front face\n";
		}
	}
	if (value == 2) {
		if (ccw) {
			ccw = false;
			glFrontFace(GL_CW);
			std::cout << "Vertices defined in clockwise direction, viewed from front face\n";
		}
		else {
			std::cout << "Vertices already defined in clockwise direction, viewed from front face\n";
		}
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
	  std::cout << "Left button and drag: ZOOM\n";
	  break;
	case 2:
	  navMode = ORBIT;  // Orbit around the model
	  std::cout << "Left button and drag: ORBIT\n";
	  break;
	case 3:
	  dist = 5.0;      // Restore initial values
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
	  std::cout << "Navigation set to slow\n";
	  break;
	case 2:
	  speed = 1.0;   // Normal
	  std::cout << "Navigation set to normal\n";
	  break;
	case 3:
	  speed = 10.0;  // Fast
	  std::cout << "Navigation set to fast\n";
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
  int axesMenu, zbufferMenu, backfaceMenu, frontfaceMenu, navmodeMenu, speedMenu, windowMenu;  

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

  axesMenu = glutCreateMenu(axesSelect);
  glutAddMenuEntry("Show", 1);
  glutAddMenuEntry("Hide", 2);

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
  glutAddSubMenu("Axes", axesMenu);
  glutAddSubMenu("Z-buffer", zbufferMenu);
  glutAddSubMenu("Back faces", backfaceMenu);
  glutAddSubMenu("Front faces", frontfaceMenu);
  glutAddSubMenu("Navigation", navmodeMenu);
  glutAddSubMenu("Speed", speedMenu);
  glutAddSubMenu("Window", windowMenu);
  glutAddMenuEntry("Exit", 666);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* Print help to console */
  printHelp();

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
