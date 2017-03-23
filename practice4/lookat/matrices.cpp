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
#include<iostream>
#include "matrices.h"
#include "glm/ext.hpp"
using namespace std;
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

//Function normalize = devuelve vector de misma direccion pero de longitud 1
//Function cross = cross product

//*******************************************************************

glm::mat4 Matrices::lookAt(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
{
	glm::mat4 ret;
	std::cout << glm::to_string(eye) << std::endl;

	glm::vec3 nVector = (eye - center) / glm::length(eye - center);
	std::cout << glm::to_string(nVector) << std::endl;
	glm::vec3 normUp = glm::normalize(up);
	glm::vec3 uVector = glm::cross(normUp, nVector) / glm::length(glm::cross(normUp, nVector));
	glm::vec3 vVector = glm::cross(nVector, uVector);
	
	//Rotation matrix
	glm::mat4 rot;
	// first column
	rot[0][0] = uVector.x;
	rot[0][1] = vVector.x;
	rot[0][2] = nVector.x;
	rot[0][3] = 0.0;
	// second column
	rot[1][0] = uVector.y;
	rot[1][1] = vVector.y;
	rot[1][2] = nVector.y;
	rot[1][3] = 0.0;
	// third column
	rot[2][0] = uVector.z;
	rot[2][1] = vVector.z;
	rot[2][2] = nVector.z;
	rot[2][3] = 0.0;
	// fourth column
	rot[3][0] = 0.0;
	rot[3][1] = 0.0;
	rot[3][2] = 0.0;
	rot[3][3] = 1.0;
	
	//Translation matrix
	glm::mat4 trans = translate(-eye.x, -eye.y, -eye.z);

	//Transformation matrix
	ret = rot * trans;
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
