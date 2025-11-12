#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "CommonValues.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader {
public:
    Shader();
    ~Shader();

    // Utility functions
    std::string ReadFile(const char* fileLocation);

    // Get uniform locations for direct OpenGL use
    GLuint GetProjectionLocation();
    GLuint GetViewLocation();
    GLuint GetModelLocation();

    GLuint GetEyePositionLocation();

    GLuint GetAmbientIntensityLocation();
    GLuint GetAmbientColourLocation();
    GLuint GetDiffuseIntensityLocation();
    GLuint GetDirectionLocation();
    GLuint GetSpecularIntensityLocation();
    GLuint GetShininessLocation();

    void CreateFromFiles(const char* vertexShaderPath, const char* fragmentShaderPath);

    // Uniform setters
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setInt(const std::string& name, int value);

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLights(PointLight* pLight, unsigned int lightCount);
	void SetSpotLights(SpotLight* sLight, unsigned int lightCount);

	void SetTexture(GLuint textureUnit);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetDirectionalLightTransform(const glm::mat4 &lTransform);

    void UseShader();
    void ClearShader();

private:
    int pointLightCount;
	int spotLightCount;

    GLuint shaderID;
    GLuint uniformProjection;
    GLuint uniformView;
    GLuint uniformModel;

    GLuint uniformEyePosition;

    GLuint uniformSpecularIntensity, uniformShininess;
	GLuint uniformTexture;
    GLuint uniformDirectionalLightTransform, uniformDirectionalShadowMap;

    struct {
        GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

        GLuint uniformDirection;

	}uniformDirectionalLight;

	GLuint uniformPointLightCount;

    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

    }uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount;

    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformLinear;
        GLuint uniformExponent;

        GLuint uniformDirection;
        GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

#endif