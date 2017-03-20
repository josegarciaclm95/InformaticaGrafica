/* matrices.cpp
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

#include "matrices.h"


// Note: when you construct a matrix using mat4(), it will be COLUMN-MAJOR

//*******************************************************************

glm::mat4 Matrices::translate(const float &tx, const float &ty, const float &tz)
{
	glm::mat4 ret;

	// Implement translation 

	// first column
	ret[0][0] = 1.0;
	ret[0][1] = 0.0;
	ret[0][2] = 0.0;
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = 0.0;
	ret[1][1] = 1.0;
	ret[1][2] = 0.0;
	ret[1][3] = 0.0;
	// third column
	ret[2][0] = 0.0;
	ret[2][1] = 0.0;
	ret[2][2] = 1.0;
	ret[2][3] = 0.0;
	// fourth column
	ret[3][0] = tx;
	ret[3][1] = ty;
	ret[3][2] = tz;
	ret[3][3] = 1.0;

	return ret;
}

//*******************************************************************

glm::mat4 Matrices::scale(const float &sx, const float &sy, const float &sz) 
{
    glm::mat4 ret;

    // Implement scaling 

	// first column
	ret[0][0] = sx;
	ret[0][1] = 0.0;
	ret[0][2] = 0.0;
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = 0.0;
	ret[1][1] = sy;
	ret[1][2] = 0.0;
	ret[1][3] = 0.0;
	// third column
	ret[2][0] = 0.0;
	ret[2][1] = 0.0;
	ret[2][2] = sz;
	ret[2][3] = 0.0;
	// fourth column
	ret[3][0] = 0.0;
	ret[3][1] = 0.0;
	ret[3][2] = 0.0;
	ret[3][3] = 1.0;

    return ret;
}

//*******************************************************************

glm::mat4 Matrices::rotateX(const float degrees)
{
	glm::mat4 ret;

	// Implement rotation about X-axis

	float radians;
	radians = glm::radians(degrees);

	// first column
	ret[0][0] = 1.0;
	ret[0][1] = 0.0;
	ret[0][2] = 0.0;
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = 0.0;
	ret[1][1] = glm::cos(radians);
	ret[1][2] = glm::sin(radians);
	ret[1][3] = 0.0;
	// third column
	ret[2][0] = 0.0;
	ret[2][1] = -glm::sin(radians);
	ret[2][2] = glm::cos(radians);
	ret[2][3] = 0.0;
	// fourth column
	ret[3][0] = 0.0;
	ret[3][1] = 0.0;
	ret[3][2] = 0.0;
	ret[3][3] = 1.0;

	return ret;
}

//*******************************************************************

glm::mat4 Matrices::rotateY(const float degrees)
{
	glm::mat4 ret;

	// Implement rotation about Y-axis 

	float radians;
	radians = glm::radians(degrees);

	// first column
	ret[0][0] = glm::cos(radians);
	ret[0][1] = 0.0;
	ret[0][2] = -glm::sin(radians);
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = 0.0;
	ret[1][1] = 1.0;
	ret[1][2] = 0.0;
	ret[1][3] = 0.0;
	// third column
	ret[2][0] = glm::sin(radians);
	ret[2][1] = 0.0;
	ret[2][2] = glm::cos(radians);
	ret[2][3] = 0.0;
	// fourth column
	ret[3][0] = 0.0;
	ret[3][1] = 0.0;
	ret[3][2] = 0.0;
	ret[3][3] = 1.0;

	return ret;
}

//*******************************************************************

glm::mat4 Matrices::rotateZ(const float degrees)
{
	glm::mat4 ret;

	// Implement rotation about Z-axis

	float radians;
	radians = glm::radians(degrees);

	// first column
	ret[0][0] = glm::cos(radians);
	ret[0][1] = glm::sin(radians);
	ret[0][2] = 0.0;
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = -glm::sin(radians);
	ret[1][1] = glm::cos(radians);
	ret[1][2] = 0.0;
	ret[1][3] = 0.0;
	// third column
	ret[2][0] = 0.0;
	ret[2][1] = 0.0;
	ret[2][2] = 1.0;
	ret[2][3] = 0.0;
	// fourth column
	ret[3][0] = 0.0;
	ret[3][1] = 0.0;
	ret[3][2] = 0.0;
	ret[3][3] = 1.0;

	return ret;
}

//*******************************************************************

glm::mat4 Matrices::lookAt(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
{
	glm::mat4 ret;
	//Function normalize = devuelve vector de misma direccion pero de longitud 1
	//Function cross = cross product
	/* ... to do ... */
	glm::mat4 trans = translate(eye.x, eye.y, eye.z);


	return ret;
}

//*******************************************************************

Matrices::Matrices()
{

}

//*******************************************************************

Matrices::~Matrices()
{

}
