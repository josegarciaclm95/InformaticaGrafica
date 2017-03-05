/* matrices.h
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

#ifndef _MATRICES_H_
#define _MATRICES_H_

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Matrices  
{
public:
	Matrices();
	virtual ~Matrices();
	static glm::mat4 translate(const float &tx, const float &ty, const float &tz);
	static glm::mat4 scale(const float &sx, const float &sy, const float &sz);
	static glm::mat4 rotateX(const float degrees);
	static glm::mat4 rotateY(const float degrees);
	static glm::mat4 rotateZ(const float degrees);
};

#endif  // !defined(_MATRICES_H_) 

