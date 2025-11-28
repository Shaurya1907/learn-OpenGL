#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
	GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensity, GLfloat dIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity)
{
	direction = glm::vec3(xDir, yDir, zDir);
	lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
	GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

// UPDATED: Support both fixed direction AND orbiting light with angle
glm::mat4 DirectionalLight::CalculateLightTransform(float angle)
{
	// Calculate light position on circular orbit around scene center (0,0,0)
	float radius = 25.0f;    // Orbit radius
	float fixedY = 15.0f;    // Keep consistent with main loop (above the scene)

	float xPos = radius * cos(angle);
	float zPos = radius * sin(angle);
	glm::vec3 lightPos(xPos, fixedY, zPos);

	// Light looks toward scene center
	glm::vec3 lookAt(0.0f, 0.0f, 0.0f);

	// Create light view matrix
	glm::mat4 lightView = glm::lookAt(lightPos, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));

	// Return complete light transform (proj * view)
	return lightProj * lightView;
}

DirectionalLight::~DirectionalLight()
{
}
