/* raycasting.cpp
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
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::lookAt, glm::perspective

#include "matrices.h"

#include "object.h"
#include "objectlist.h"
#include "view.h"
#include "frame.h"


/*******************/
/* Data structures */
/*******************/

/* List of spheres */
ObjectList objects;

/* Viewpoint parameters */
glm::vec3 eye,     // Observer 
         center,  // Looking at this point
		 up;      // View-up vector

/* Variables related to Ray Casting method */
glm::vec3 bgcolor;  // Background color
View view;         // View parameters
Frame frame;       // Frame buffer

bool raycasting = false;

#define TFAR 10000;  // Maximum t value

/* Window size */
int width;   // Width
int height;  // Height

/* Array of characters */
char buffer[256];


/***********************/
/* Function prototypes */
/***********************/

void init(void);
void output(GLfloat x, GLfloat y, char* text);
void drawAxes(GLfloat length);
void drawSpheres(void);
void drawHUD(void);
void RayCasting(void);
void redraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);


//*******************************************************************

void
init(void)
{  
  Sphere *addme;
  glm::mat4 lookatMat;

  /* Add spheres to the object list */
  addme = new Sphere(glm::vec3(0.0, 0.0, 0.0), 0.5, glm::vec3(1.0, 0.0, 0.0));
  objects.Add(addme);
  addme = new Sphere(glm::vec3(-0.5, 0.25, 1.0), 0.25, glm::vec3(0.0, 1.0, 0.0));
  objects.Add(addme);
  addme = new Sphere(glm::vec3(0.5, 0.5, -1.0), 0.75, glm::vec3(0.0, 0.0, 1.0));
  objects.Add(addme);

  /* Set black as background colour, RGB(0,0,0) */
  bgcolor = glm::vec3(0.0, 0.0, 0.0);

  /* Set viewpoint parameters */
  eye = glm::vec3(0.0, 0.0, 5.0);     // Observer at (0,0,5)
  center = glm::vec3(0.0, 0.0, 0.0);  // Looking at point (0,0,0)
  up = glm::vec3(0.0, 1.0, 0.0);      // Up vector pointing in the positive Y direction

  /* OpenGL */

  glClearColor(bgcolor.x, bgcolor.y, bgcolor.z, 0.0);

  glEnable(GL_DEPTH_TEST); // Enable depth buffering

  glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //gluLookAt(eye.x, eye.y, eye.z,   
  //          center.x, center.y, center.z,   
  //          up.x, up.y, up.z);
  lookatMat = glm::lookAt(eye, center, up);
  glLoadMatrixf(&lookatMat[0][0]);

  /* Ray Casting */

  view.SetLookAt(eye, center, up);
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
drawSpheres(void)
{
  Object *optr;

  /* Access and draw the objects in the list in sequence */
  optr = objects.First();
  while (optr != NULL) {
	optr->Draw();
    optr = objects.Next();
  }
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
  
  glColor3f(1.0, 1.0, 1.0);
  sprintf_s(buffer, "Press space bar to execute Ray Casting (and wait!)");
  output(10, 15, buffer);

  /* Restore previous projection and modelview matrices */
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

//*******************************************************************

void
RayCasting(void)
{
  long numObjs = 0, numRays = 0, numTests = 0;
  float pixelWidth, pixelHeight;

  fprintf(stdout, "Executing Ray Casting\n");

  /* Pixel width and height */
  pixelWidth = (view.umax - view.umin)/(float)width;
  pixelHeight = (view.vmax - view.vmin)/(float)height;

  for (int y=0; y<height; y++) {

    for (int x=0; x<width; x++) {

	  Object *optr, *nearObj = NULL;
      float t, tnear = TFAR;
      glm::vec3 dirN, dirU, dirV, dir;

      /* Direction towards the pixel */
      dirN = view.n*(view.d);
	  dirU = view.u*(view.umin + x*pixelWidth + pixelWidth/2.0f);
      dirV = view.v*(view.vmin + y*pixelHeight + pixelHeight/2.0f);
	  dir =  dirN + dirU + dirV;

      /* Access the object list in sequence to find
	     the nearest ray-object intesection */
	  optr = objects.First();
      while (optr != NULL) {

		/* Compute ray-object intersection (t),
		   save the nearest one so far (tnear, nearObj) */
		  t = optr->NearestInt(eye, dir);
		  if (t > tnear) {
			  tnear = t;
			  nearObj = optr;
		  }

		/* ... to do ... */

        optr = objects.Next();
	  }

	  /* Paint the pixel (SetPixel) with the color of the nearest
	     object (ComputeColor) or with the background if the ray misses */
	  if (nearObj == nullptr) {
		  frame.SetPixel(x, y, bgcolor);
	  }
	  else {
		  frame.SetPixel(x, y, nearObj->ComputeColor(eye, dir, tnear));
	  }
	  
      /* ... to do ... */
	}

    fprintf(stdout, "." );
  }

  /* Set projection and modelview matrices to draw using OpenGL */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width-1, 0, height-1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  frame.Draw();  // Draw the image in the OpenGL context

  glColor3f(1.0, 1.0, 1.0);
  sprintf_s(buffer, "Press space bar to execute the Z-buffer method");
  output(10, 15, buffer);

  /* Restore previous projection and modelview matrices */
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  /* Print statistics */
  numObjs = objects.Length();
  numRays = width * height;
  numTests = numRays * numObjs;
  fprintf(stdout, "\n");
  fprintf(stdout, "Number of objects: %d\n", numObjs);
  fprintf(stdout, "Number of rays: %d\n", numRays);
  fprintf(stdout, "Number of tests: %d\n", numTests);
}

//*******************************************************************

void
redraw(void)
{
  if (!raycasting) {

    /* Clear frame and depth buffer bits */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Draw reference axes */
    drawAxes(5.0);  

    /* Draw model */
	drawSpheres();

    /* Draw on-screen information */
    drawHUD();

  } else {

	/* Execute Ray Casting */
	RayCasting();
  }

  /* Swap back and front buffers */
  glutSwapBuffers();
}

//*******************************************************************

void
reshape(int w, int h)
{
	glm::mat4 projectionMat;

	/* OpenGL */

	/* Viewport is same size as the application window */
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	/* Set the projection matrix as the active one */
	glMatrixMode(GL_PROJECTION);

	/* Set perspective projection parameters. Use the
	perspective function from the GLM library */
	//glLoadIdentity();
	//gluPerspective(40.0,         /* FOV, field of view
	//                                (vertical), in degrees */
	//               (float) w/h,  /* Aspect ratio width:height */
	//               0.1, 10000.0  /* Front face (Z near, hither) and rear face
	//                                (Z far, yon) of the viewing volume */
	//              );
	projectionMat = glm::perspective(
		glm::radians(40.0f),  /* FOV, field of view (vertical), in radians */
		(float)w / h,		  /* Aspect ratio width:height */
		0.1f, 10000.0f		  /* Front face (Z near, hither) and rear face
							  (Z far, yon) of the viewing volume */
	);
	glLoadMatrixf(&projectionMat[0][0]);

	/* Restore modelview matrix as the active one */
	glMatrixMode(GL_MODELVIEW);

	/* Ray Casting */

	/* The image is also same size as the application window */
	frame.SetSize(w, h);

	/* Perspective projection parameters are the same too */
	view.SetPerspective(40.0f, (float)w/h);

	/* Save current window size */
	width = w;
	height = h;
}

//*******************************************************************

void 
keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case ' ':  // Switch between Z-buffer and Ray Casting methods
	  if (!raycasting) {
		glDisable(GL_DEPTH_TEST);
		raycasting = true;
	  } else {
		raycasting = false;
		glEnable(GL_DEPTH_TEST);
	  }
      glutPostRedisplay();
      break;
    case 27:  // Escape key
      exit(0);
  }
}

//*******************************************************************

void
main(int argc, char* argv[])
{
  /* Process the command line and negotiates the start
	 of an OpenGL session with the window system */
  glutInit(&argc, argv);
  
  /* Set the OpenGL display mode for the window that will be created
     next: doble buffer, RGBA colour model, and depth buffer */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

  /* Create a window for OpenGL rendering: selects the appropriate visual
     and colourmap, and create an OpenGL context associated to this window */
  glutInitWindowSize(640, 480);
  glutCreateWindow("raycasting");

  /* Initialize GLEW */
  GLenum err = glewInit();
  if (GLEW_OK != err) {
	  std::cerr << "Error: " << glewGetString(err) << std::endl;
  }

  /* Initialize the OpenGL context */
  init();

  /* Register event handler functions */

  /* Called each time the window needs to be redisplayed */ 
  glutDisplayFunc(redraw);

  /* Called each time the window is moved or resized */
  glutReshapeFunc(reshape);

  /* Called when a key that generates an ASCII code is pressed */
  glutKeyboardFunc(keyboard);

  /* Event loop */
  glutMainLoop();
}
