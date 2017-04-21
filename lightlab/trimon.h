/* trimon.h
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

#ifndef _TRIMON_H_
#define _TRIMON_H_

/***********************/
/* Function prototypes */
/***********************/

void trimonEnableStereo(void);
void trimonPreStereoImg(int width, int height);
void trimonPreLeftImg(void);
void trimonPreRightImg(void);
void trimonPostStereoImg(void);
void trimonDisableStereo(void);

#endif  // !defined(_TRIMON_H_) 