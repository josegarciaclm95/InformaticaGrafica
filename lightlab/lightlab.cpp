/* lightlab.cpp
 *
 * Informatica Grafica / Computer Graphics.
 * Spring 2017.
 *
 * Practice 6.
 * Lighting in OpenGL.
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
#include "GL/glew.h"  /* OpenGL Extension Wrangler*/
#include "GL/glut.h"  /* OpenGL Utility Toolkit */

/* Shader loader, from the red book */
#include "LoadShaders.h"

/* GLM */
#include "glm/vec3.hpp" // glm::vec3
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale

/* Our own matrix library */
#include "matrices.h"

/* Zalman Trimon 3D display */
#include "trimon.h"
#define PI 3.14159265358979323846264338327950288   /* pi */


/*******************/
/* Data structures */
/*******************/

/* Window size */
int width;   // Width
int height;  // Height

/* View menu options */
bool viewAxes = true, viewHUD = true;

/* Navigation modes */
enum navMode_type { ZOOM, ORBIT } navMode;
				 
float dist = 10.0;  // Distance to object
float orbitX = 0.0, 
      orbitY = 0.0; // Rotation angles, in degrees,
					//   about X and Y axes
float speed = 1.0;  // Navigation speed

/* Store pointer coordinates and
   the state of mouse left button */
int lastX = 0, lastY = 0;
bool isLeftButtonPressed = false;

/* Rotation angle (for animation) */
GLfloat angle = 0.0;  
GLfloat degree = PI / 180.0f;
/* Shader variables */
GLuint render_prog;

// Variables to set uniform params for lighting fragment shader 
GLuint enablelighting_loc;
GLuint lightposn_loc;
GLuint lightambient_loc;
GLuint lightdiffuse_loc;
GLuint lightspecular_loc;
GLuint lightenabled_loc;
GLuint numused_loc;

GLuint ambient_loc;
GLuint diffuse_loc;
GLuint specular_loc;
GLuint emission_loc;
GLuint shininess_loc;

/* Application lighting variables */
bool enablelighting, enablewhitelight, 
  enablegreenlight, enableredlight;


/***********************/
/* Function prototypes */
/***********************/

void printHelp(void);
void init(void);
void drawModel(void);
void redraw(void);
void stereoRedraw(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle(void);
void lightingSelect(int value);
void whitelightSelect(int value);
void greenlightSelect(int value);
void redlightSelect(int value);
void navmodeSelect(int value);
void speedSelect(int value);
void windowSelect(int value);
void trimonSelect(int value);
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
	if (enablelighting)
		std::cout << "Lighting enabled\n";
	else
		std::cout << "Lighting disabled\n";
	if (enablewhitelight)
		std::cout << "White light enabled\n";
	else
		std::cout << "White light disabled\n";
	if (enablegreenlight)
		std::cout << "Green light enabled\n";
	else
		std::cout << "Green light disabled\n";
	if (enableredlight)
		std::cout << "Red light enabled\n";
	else
		std::cout << "Red light disabled\n";
	if (navMode == ZOOM)
		std::cout << "Left button and drag: ZOOM\n";
	if (navMode == ORBIT)
		std::cout << "Left button and drag: ORBIT\n";
	std::cout << "\n";
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
	  { GL_VERTEX_SHADER, "./shaders/lightlab.vert.glsl" },
	  { GL_FRAGMENT_SHADER, "./shaders/lightlab.frag.glsl" },
	  { GL_NONE, NULL }
  };
  render_prog = LoadShaders(shader_info);
  glUseProgram(render_prog);  // Activate shading program

  enablelighting_loc = glGetUniformLocation(render_prog, "enablelighting");
  lightposn_loc = glGetUniformLocation(render_prog, "lightposn");
  lightambient_loc = glGetUniformLocation(render_prog, "lightambient");
  lightdiffuse_loc = glGetUniformLocation(render_prog, "lightdiffuse");
  lightspecular_loc = glGetUniformLocation(render_prog, "lightspecular");
  lightenabled_loc = glGetUniformLocation(render_prog, "lightenabled");
  numused_loc = glGetUniformLocation(render_prog, "numused");

  ambient_loc = glGetUniformLocation(render_prog, "ambient");
  diffuse_loc = glGetUniformLocation(render_prog, "diffuse");
  specular_loc = glGetUniformLocation(render_prog, "specular");
  emission_loc = glGetUniformLocation(render_prog, "emission");
  shininess_loc = glGetUniformLocation(render_prog, "shininess");

  /* Enable depth buffering */
  glEnable(GL_DEPTH_TEST);

  /* Enable back-face culling */
  glDisable(GL_CULL_FACE);

  /* Set modelview matrix as the active one */
  glMatrixMode(GL_MODELVIEW);

  navMode = ORBIT;

  enablelighting = true;
  enablewhitelight = true;
  enablegreenlight = true;
  enableredlight = true;
}

//*******************************************************************

void
drawModel(void)
{
	glm::mat4 viewMat, modelviewMat;
	glGetFloatv(GL_MODELVIEW_MATRIX, &viewMat[0][0]);

	// Lighting parameter arrays, similar to that in the fragment shader

	const int maxLights = 10;

	// Lights: positions
	static const GLfloat lightposn[] = {
		0.0f, 0.0f, 10.0f, 1.0f,  // Light 1, point
		-10.0f, 0.0f, 10.0f, 1.0f,
		10.0f, 0.0f, 0.0f, 1.0f
	};
	// Lights: ambient
	static const GLfloat lightambient[] = {
		0.1f, 0.1f, 0.1f, 1.0f,  // Light 1, white
		0.1f, 0.0f, 0.0f, 1.0f,  // Light 2, red
		0.0f, 0.1f, 0.0f, 1.0f,  // Light 3, green
	};
	// Lights: diffuse
	static const GLfloat lightdiffuse[] = {
		1.0f, 1.0f, 1.0f, 1.0f,  // Light 1, white
		1.0f, 0.0f, 0.0f, 1.0f,  // Light 2, red
		0.0f, 1.0f, 0.0f, 1.0f,  // Light 3, green
	};
	// Light: specular
	static const GLfloat lightspecular[] = {
		1.0f, 1.0f, 1.0f, 1.0f,  // Light 1, white
		1.0f, 0.0f, 0.0f, 1.0f,  // Light 2, red
		0.0f, 1.0f, 0.0f, 1.0f,  // Light 3, green
	};
	// Lights: enabled
	GLint lightenabled[] = {
		enablewhitelight,
		enableredlight,
		enablegreenlight,
		
	};
	// Lights: how many are used
	int numused = 3;	

	// Lights are transformed by current modelview matrix. 
	// The shader can't do this globally, so we need to do so manually.

	GLfloat lightransf[4 * maxLights];  // Light position transformed by modelview
	if (enablelighting) {
		glUniform1i(enablelighting_loc, true);

		// Transform by current modelview matrix
		glm::vec4 aux;
		for (int i = 0, offset = 0; i < numused; i++, offset += 4) {
			aux = viewMat * glm::vec4(lightposn[offset], lightposn[offset + 1], lightposn[offset + 2], lightposn[offset + 3]);
			lightransf[offset] = aux.x; lightransf[offset + 1] = aux.y; lightransf[offset + 2] = aux.z; lightransf[offset + 3] = aux.w;
		}

		// Pass position and color to the shader
		glUniform4fv(lightposn_loc, numused, lightransf);
		glUniform4fv(lightambient_loc, numused, lightambient);
		glUniform4fv(lightdiffuse_loc, numused, lightdiffuse);
		glUniform4fv(lightspecular_loc, numused, lightspecular);
		glUniform1iv(lightenabled_loc, numused, lightenabled);
		glUniform1i(numused_loc, numused);
	}
	else {
		glUniform1i(enablelighting_loc, false);

		glColor3f(1.0, 1.0, 1.0);
	}

	// Torus, red plastic material

	GLfloat redplasticAmbient[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat redplasticDiffuse[4] = { 0.5, 0.0, 0.0, 1.0 };
	GLfloat redplasticSpecular[4] = { 0.7, 0.6, 0.6, 1.0 };
	GLfloat redplasticEmission[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat redplasticShininess = 27.0;

	glUniform4fv(ambient_loc, 1, redplasticAmbient);
	glUniform4fv(diffuse_loc, 1, redplasticDiffuse);
	glUniform4fv(specular_loc, 1, redplasticSpecular);
	glUniform4fv(emission_loc, 1, redplasticEmission);
	glUniform1fv(shininess_loc, 1, &redplasticShininess);

	glm::mat4 torusMat, RT_X, RT_Y, RT;
	RT = Matrices::translate(0.0f, 1.0f, 0.0f);
	RT_X = Matrices::rotateX(45);
	RT_Y = Matrices::rotateY(angle);
	modelviewMat = viewMat * torusMat * RT * RT_Y * RT_X;
	glLoadMatrixf(&modelviewMat[0][0]);
	glutSolidTorus(0.5, 1.0, 20, 20);  // innerRadius, outerRadius, nsides, rings

	// Utah teapot, Brass material
	GLfloat brassAmbient[4] = { 0.33, 0.22, 0.03, 1.0 };
	GLfloat brassDiffuse[4] = { 0.78, 0.57, 0.11, 1.0 };
	GLfloat brassSpecular[4] = { 0.99, 0.91, 0.81, 1.0 };
	GLfloat brassEmission[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat brassShininess = 32.0;
	//Link to shader
	glUniform4fv(ambient_loc, 1, brassAmbient);
	glUniform4fv(diffuse_loc, 1, brassDiffuse);
	glUniform4fv(specular_loc, 1, brassSpecular);
	glUniform4fv(emission_loc, 1, brassEmission);
	glUniform1fv(shininess_loc, 1, &brassShininess);

	//Utah Teapot transformations
	glm::mat4 teapotMat, RTP_Y, TTP;

	RTP_Y = Matrices::rotateY(angle);
	TTP = Matrices::translate(-2.5f, -1.0f, 0.0f);
	modelviewMat = viewMat * teapotMat * TTP * RTP_Y;
	glLoadMatrixf(&modelviewMat[0][0]);
	glFrontFace(GL_CW);
	glutSolidTeapot(1.0); 
	glFrontFace(GL_CCW);

	// Icosahedron, emerald material
	GLfloat emeraldAmbient[4] = { 0.02, 0.17, 0.02, 1.0 };
	GLfloat emeraldDiffuse[4] = { 0.07, 0.61, 0.07, 1.0 };
	GLfloat emeraldSpecular[4] = { 0.63, 0.72, 0.63, 1.0 };
	GLfloat emeraldEmission[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat emeraldShininess = 76.8;
	//Link to shader
	glUniform4fv(ambient_loc, 1, emeraldAmbient);
	glUniform4fv(diffuse_loc, 1, emeraldDiffuse);
	glUniform4fv(specular_loc, 1, emeraldSpecular);
	glUniform4fv(emission_loc, 1, emeraldEmission);
	glUniform1fv(shininess_loc, 1, &emeraldShininess);

	glm::mat4 icoMat, RI_Y, TI;

	RI_Y = Matrices::rotateY(angle);
	TI = Matrices::translate(2.5f, -1.0f, 0.0f);
	modelviewMat = viewMat * icoMat * TI * RI_Y;
	glLoadMatrixf(&modelviewMat[0][0]);
	glutSolidIcosahedron();

	glLoadMatrixf(&viewMat[0][0]);
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

	/* Draw model */
	drawModel();

	/* Swap back and front buffers */
	glutSwapBuffers();
}

//*******************************************************************

void
stereoRedraw(void)
{
  glm::mat4 lookatMat, orbitMat, viewMat;
  GLfloat eyesep;  // Eye separation, interpupillary or interocular distance
  GLfloat offset;
  int i;

  eyesep = dist/15.0;

  /* Draw stencial pattern for the interleaved stereo mode */
  trimonPreStereoImg(width, height);

  /* Clear frame and depth buffer bits */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  offset = -eyesep/2.0;  /* Left eye at (-eyesep/2,0,dist) */
  for (i=0; i<2; i++) {
    if (i == 0) {
      /* Left eye, even rows */
      trimonPreLeftImg();
	} else {
      /* Right eye, odd rows */
      trimonPreRightImg();
	}

	/* Set viewpoint, store the matrix in lookatMat */
	//lookatMat = glm::lookAt(
	//	glm::vec3(-eyesep/2.0f, 0.0f, dist),
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 1.0f, 0.0f)
	//);
	lookatMat = Matrices::lookAt(
		glm::vec3(offset, 0.0f, dist),  /* Left pupil at (-eyesep/2,0,dist) */
		glm::vec3(0.0f, 0.0f, 0.0f),	/* Looking at point (0,0,0) */
		glm::vec3(0.0f, 1.0f, 0.0f)		/* Up vector pointing in the positive Y direction */
	);

	/* Navigation transformations */
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitX), glm::vec3(1.0, 0.0, 0.0));
	//orbitMat = glm::rotate(orbitMat, glm::radians(-orbitY), glm::vec3(0.0, 1.0, 0.0));
	orbitMat = Matrices::rotateX(-orbitX) * Matrices::rotateY(-orbitY);

	viewMat = lookatMat * orbitMat;
	glLoadMatrixf(&viewMat[0][0]);

    /* Draw model */
    drawModel();

    offset += eyesep;  /* Right eye at (+eyesep/2,0,dist) */
  }

  /* Masks are no longer needed */
  trimonPostStereoImg();

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

  /* Set perspective projection parameters */
  glMatrixMode(GL_PROJECTION);
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
idle(void)
{
	angle += degree*20;
	glutPostRedisplay();
}

//*******************************************************************

void
lightingSelect(int value)
{
	if (value == 1) {
		if (!enablelighting) {
			enablelighting = true;
			std::cout << "Lighting enabled\n";
		}
		else {
			std::cout << "Lighting already enabled\n";
		}
	}
	if (value == 2) {
		if (enablelighting) {
			enablelighting = false;
			std::cout << "Lighting disabled\n";
		}
		else {
			std::cout << "Lighting already disabled\n";
		}
	}
	glutPostRedisplay();
}

//*******************************************************************

void
whitelightSelect(int value)
{
	if (value == 1) {
		if (!enablewhitelight) {
			enablewhitelight = true;
			std::cout << "White light enabled\n";
		}
		else {
			std::cout << "White light already enabled\n";
		}
		std::cout << "White light enabled\n";
	}
	if (value == 2) {
		if (enablewhitelight) {
			enablewhitelight = false;
			std::cout << "White light disabled\n";
		}
		else {
			std::cout << "White light already disabled\n";
		}
	}
	glutPostRedisplay();
}

//*******************************************************************

void
greenlightSelect(int value)
{
	if (value == 1) {
		if (!enablegreenlight) {
			enablegreenlight = true;
			std::cout << "Green light enabled\n";
		}
		else {
			std::cout << "Green light already enabled\n";
		}
	}
	if (value == 2) {
		if (enablegreenlight) {
			enablegreenlight = false;
			std::cout << "Green light disabled\n";
		}
		else {
			std::cout << "Green light already disabled\n";
		}
	}
	glutPostRedisplay();
}

//*******************************************************************

void
redlightSelect(int value)
{
	if (value == 1) {
		if (!enableredlight) {
			enableredlight = true;
			std::cout << "Red light enabled\n";
		}
		else {
			std::cout << "Red light already enabled\n";
		}
	}
	if (value == 2) {
		if (enableredlight) {
			enableredlight = false;
			std::cout << "Red light disabled\n";
		}
		else {
			std::cout << "Red light already disabled\n";
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
trimonSelect(int value)
{
  if (value == 1) {
      glClearColor(0.7, 0.7, 0.7, 0.0);
      trimonEnableStereo();
      glutDisplayFunc(stereoRedraw);
	  glutFullScreen();
  }
  if (value == 2) {
      glClearColor(0.0, 0.0, 0.0, 0.0);
      trimonDisableStereo();
      glutDisplayFunc(redraw);
  }
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
	int lightingMenu, whitelightMenu, greenlightMenu, redlightMenu, 
	  navmodeMenu, speedMenu, windowMenu, trimonMenu;  

  /* Process the command line and negotiates the start
	 of an OpenGL session with the window system */
  glutInit(&argc, argv);
  
  /* Set the OpenGL display mode for the window that will be
     created next: doble buffer, RGBA colour model, depth buffer and stencil buffer */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);

  /* Create a window for OpenGL rendering: selects the appropriate visual
     and colourmap, and create an OpenGL context associated to this window */
  glutInitWindowSize(640, 480);
  glutCreateWindow("lightlab");

  /* Initialize GLEW */
  GLenum err = glewInit();
  if (GLEW_OK != err) {
	  std::cerr << "Error: " << glewGetString(err) << std::endl;
  }

  /* Initialize the OpenGL context */
  init();

  /* Create a pop-up menu with several submenus */

  lightingMenu = glutCreateMenu(lightingSelect);
  glutAddMenuEntry("Enable", 1);
  glutAddMenuEntry("Disable", 2);

  whitelightMenu = glutCreateMenu(whitelightSelect);
  glutAddMenuEntry("Enable", 1);
  glutAddMenuEntry("Disable", 2);

  greenlightMenu = glutCreateMenu(greenlightSelect);
  glutAddMenuEntry("Enable", 1);
  glutAddMenuEntry("Disable", 2);

  redlightMenu = glutCreateMenu(redlightSelect);
  glutAddMenuEntry("Enable", 1);
  glutAddMenuEntry("Disable", 2);

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

  trimonMenu = glutCreateMenu(trimonSelect);
  glutAddMenuEntry("Enable stereo", 1);
  glutAddMenuEntry("Disable estereo", 2);

  glutCreateMenu(popupmenu);
  glutAddSubMenu("Lighting", lightingMenu);
  glutAddSubMenu("White light", whitelightMenu);
  glutAddSubMenu("Green light", greenlightMenu);
  glutAddSubMenu("Red light", redlightMenu);
  glutAddSubMenu("Navigation", navmodeMenu);
  glutAddSubMenu("Speed", speedMenu);
  glutAddSubMenu("Window", windowMenu);
  glutAddSubMenu("Trimon", trimonMenu);
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

  /* Called when there is nothing else to do */
  glutIdleFunc(idle);

  /* Event loop */
  glutMainLoop();
}
