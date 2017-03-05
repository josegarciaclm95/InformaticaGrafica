/* trimon.cpp
 *
 * Informatica Grafica / Computer Graphics.
 * Spring 2017.
 *
 * Practice 3.
 * Transformations in OpenGL.
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
#include <ctype.h>

/* Microsoft Visual C++ */
#include <windows.h>

/* OpenGL */
#include <GL/gl.h>    /* OpenGL (window system independent) */
#include <GL/glu.h>   /* OpenGL Utility library */


/**********************/
/* Function protoypes */
/**********************/

void trimonEnableStereo(void);
void trimonPreStereoImg(int width, int height);
void trimonPreLeftImg(void);
void trimonPreRightImg(void);
void trimonPostStereoImg(void);
void trimonDisableStereo(void);


//*******************************************************************

void
trimonEnableStereo(void)
{
  glClearStencil(0x0);
  glEnable(GL_STENCIL_TEST);
}

//*******************************************************************

void
trimonPreStereoImg(int width, int height)
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
trimonPreLeftImg(void)
{
  glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
}

//*******************************************************************

void
trimonPreRightImg(void)
{
  glStencilFunc(GL_EQUAL, 0x1, 0x1);
}

//*******************************************************************

void
trimonPostStereoImg(void)
{
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
}

//*******************************************************************

void
trimonDisableStereo(void)
{
  glDisable(GL_STENCIL_TEST);
}

//*******************************************************************
