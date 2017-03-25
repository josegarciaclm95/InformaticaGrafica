/* object.h
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

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "glm/vec3.hpp" // glm::vec3


/****************/
/* Object class */
/****************/

class Object {

public:

  /* Computes the nearest intersection point between a ray and the object, 
     in the direction of the ray. Returns the t value of that point if it
     is positive, otherwise returns 0. */
  virtual float NearestInt(const glm::vec3& pos, const glm::vec3& dir) = 0;

  /* Returns the color of the object at the intersection point with the ray */
  virtual glm::vec3 ComputeColor(const glm::vec3& pos, const glm::vec3& dir, float t) = 0;

  /* Draw the object with OpenGL */
  virtual void Draw() = 0;
};


/****************/
/* Sphere class */
/****************/

class Sphere : public Object {

public:

  /* Constructor */
  Sphere(const glm::vec3& Center, float Radius, const glm::vec3& Color);

  /* Inherited functions that are implemented in this class */
  virtual float NearestInt(const glm::vec3& pos, const glm::vec3& dir);
  virtual glm::vec3 ComputeColor(const glm::vec3& pos, const glm::vec3& dir, float t);
  virtual void Draw();

private:

  glm::vec3 center;  // Center coordinates
  float radius;      // Radious
  glm::vec3 color;   // Color
};


#endif  // !defined _OBJECT_H_
