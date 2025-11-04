#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "CommonValues.h"

#include "DirectionalLight.h"
#include "PointLight.h"

class Shader {
public:
    Shader();
    ~Shader();

    void CreateFromFiles(const char* vertexShaderPath, const char* fragmentShaderPath);
    void UseShader();
    void ClearShader();

    // Uniform setters
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setInt(const std::string& name, int value);

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLights(PointLight* pLight, unsigned int lightCount);

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

private:
    int pointLightCount;

    GLuint shaderID;
    GLuint uniformProjection;
    GLuint uniformView;
    GLuint uniformModel;

    GLuint uniformEyePosition;

    GLuint uniformSpecularIntensity, uniformShininess;

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

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

#endif