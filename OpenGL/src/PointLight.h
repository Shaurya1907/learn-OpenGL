#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	PointLight(GLuint red, GLuint green, GLuint blue,
		GLuint aIntensity, GLuint dIntensity,
		GLuint xPos, GLuint yPos, GLuint zPos,
		GLuint con, GLuint lin, GLuint exp);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation,
		GLuint constantLocation, GLuint linearLocation,
		GLuint exponentLocation);

	~PointLight();

protected:
	glm::vec3 position;

	GLfloat constant, linear, exponent;
};