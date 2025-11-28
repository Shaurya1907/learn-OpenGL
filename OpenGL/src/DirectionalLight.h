#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void SetDirection(const glm::vec3& dir) { direction = dir; }
	const glm::vec3& GetDirection() const { return direction; }

	// UPDATED: Add angle parameter for orbiting light
	glm::mat4 CalculateLightTransform(float angle = 0.0f);

	~DirectionalLight();

	glm::mat4 lightProj;
	glm::vec3 direction;
};
