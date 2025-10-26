#pragma once

#include<glad/glad.h>
#include<glm/glm.hpp>
class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation);

	~Light();

private:
	glm::vec3 colour;
	GLfloat ambientIntensity;

};

