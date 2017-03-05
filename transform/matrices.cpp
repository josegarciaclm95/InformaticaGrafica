/* matrices.cpp
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
//NOTA MENTAL: LAS FUNCIONES SENO, COSENO, ETC. TRABAJAN CON RADIANES. HAY UNA CONSTANTE PI MAS ABAJO

#include "matrices.h"


// Note: when you construct a matrix using mat4(), it will be COLUMN-MAJOR

//*******************************************************************

glm::mat4 Matrices::translate(const float &tx, const float &ty, const float &tz)
{
	glm::mat4 ret;

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
	//third column
	ret[2][0] = 0.0;
	ret[2][1] = 0.0;
	ret[2][2] = 1.0;
	ret[2][3] = 0.0;
	//forth column
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
	//third column
	ret[2][0] = 0.0;
	ret[2][1] = 0.0;
	ret[2][2] = sz;
	ret[2][3] = 0.0;
	//forth column
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

	// first column
	ret[0][0] = 1.0;
	ret[0][1] = 0.0;
	ret[0][2] = 0.0;
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = 0.0;
	ret[1][1] = cos(degrees);
	ret[1][2] = sin(degrees);
	ret[1][3] = 0.0;
	//third column
	ret[2][0] = 0.0;
	ret[2][1] = -sin(degrees);
	ret[2][2] = cos(degrees);
	ret[2][3] = 0.0;
	//forth column
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

	// first column
	ret[0][0] = cos(degrees);
	ret[0][1] = 0.0;
	ret[0][2] = -sin(degrees);
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = 0.0;
	ret[1][1] = 1.0;
	ret[1][2] = 0.0;
	ret[1][3] = 0.0;
	//third column
	ret[2][0] = sin(degrees);
	ret[2][1] = 0.0;
	ret[2][2] = cos(degrees);
	ret[2][3] = 0.0;
	//forth column
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

	// first column
	ret[0][0] = cos(degrees);
	ret[0][1] = sin(degrees);
	ret[0][2] = 0.0;
	ret[0][3] = 0.0;
	// second column
	ret[1][0] = -sin(degrees);
	ret[1][1] = cos(degrees);
	ret[1][2] = 0.0;
	ret[1][3] = 0.0;
	//third column
	ret[2][0] = 0.0;
	ret[2][1] = 0.0;
	ret[2][2] = 1.0;
	ret[2][3] = 0.0;
	//forth column
	ret[3][0] = 0.0;
	ret[3][1] = 0.0;
	ret[3][2] = 0.0;
	ret[3][3] = 1.0;

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
