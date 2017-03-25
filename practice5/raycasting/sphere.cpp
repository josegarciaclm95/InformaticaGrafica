/* sphere.cpp
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


#include <windows.h>

#include "GL/glew.h"
#include "GL/glut.h"

#include "glm/glm.hpp" // glm::vec3, glm::dot
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale

#include "object.h"


/* Constructor */
Sphere::Sphere(const glm::vec3& Center, float Radius, const glm::vec3& Color) {
  center = Center;
  radius = Radius;
  color = Color;
}

/* Computes the nearest intersection point between a ray and the sphere, 
   in the direction of the ray. Returns the t value of that point if it
   is positive, otherwise returns 0. */
float Sphere::NearestInt(const glm::vec3& pos, const glm::vec3& dir)
{
	/* ... to do ... */
}

/* Returns the color of the sphere at the intersection point with the ray */
glm::vec3 Sphere::ComputeColor(const glm::vec3& pos, const glm::vec3& dir, float t) {

	return color;
}

/* Draw the object with OpenGL and GLUT */
void Sphere::Draw() {
	glm::mat4 viewMat, sphereMat, modelviewMat;
	glGetFloatv(GL_MODELVIEW_MATRIX, &viewMat[0][0]);

	sphereMat = glm::translate(sphereMat, glm::vec3(center.x, center.y, center.z));
	modelviewMat = viewMat * sphereMat;
	glLoadMatrixf(&modelviewMat[0][0]);
	glColor3f(color.x, color.y, color.z);
	glutSolidSphere(radius, 20, 20);

	glLoadMatrixf(&viewMat[0][0]);
}
