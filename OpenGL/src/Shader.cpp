#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
{
    shaderID = 0;
    uniformProjection = 0;
    uniformView = 0;
    uniformModel = 0;

    pointLightCount = 0;
}

void Shader::CreateFromFiles(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    std::string vertexString = ReadFile(vertexShaderPath);
    std::string fragmentString = ReadFile(fragmentShaderPath);
    std::cout << "[Shader] Loaded vertex (" << vertexShaderPath << ") bytes=" << vertexString.size() << "\n";
    std::cout << "[Shader] Loaded fragment (" << fragmentShaderPath << ") bytes=" << fragmentString.size() << "\n";

    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);
    if (!fileStream.is_open()) {
        std::cout << "[Shader] Could not read file " << fileLocation << ". File does not exist.\n";
        return "";
    }

    std::string line;
    while (std::getline(fileStream, line)) {
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
    shaderID = glCreateProgram();

    if (!shaderID) {
        std::cout << "Error creating shader program!" << std::endl;
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout << "Error linking program: " << eLog << std::endl;
        return;
    }

    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout << "Error validating program: " << eLog << std::endl;
        return;
    }

    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformModel = glGetUniformLocation(shaderID, "model");

    // These must match fragment_core1.glsl exactly
    uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.diffuseIntensity");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
	uniformShininess = glGetUniformLocation(shaderID, "material.shininess");

	uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

	uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");
    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
		char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].base.colour", i);
		uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%zu].exponent", i);
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
    }
}

void Shader::SetPointLights(PointLight* pLight, unsigned int lightCount)
{
    if(lightCount > MAX_POINT_LIGHTS)
		lightCount = MAX_POINT_LIGHTS;

    glUniform1i(uniformPointLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++)
    {
         pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity,
                            uniformPointLight[i].uniformColour,
                            uniformPointLight[i].uniformDiffuseIntensity,
                            uniformPointLight[i].uniformPosition,
                            uniformPointLight[i].uniformConstant,
                            uniformPointLight[i].uniformLinear,
			                uniformPointLight[i].uniformExponent);
    }
}

void Shader::UseShader()
{
    glUseProgram(shaderID);
}

void Shader::ClearShader()
{
    if (shaderID != 0) {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }
    uniformModel = 0;
    uniformView = 0;
    uniformProjection = 0;
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = static_cast<GLint>(strlen(shaderCode));

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        std::cout << "Error compiling the " << shaderType << " shader: " << eLog << std::endl;
        return;
    }

    glAttachShader(theProgram, theShader);
}

GLuint Shader::GetProjectionLocation() { return uniformProjection; }
GLuint Shader::GetViewLocation() { return uniformView; }
GLuint Shader::GetModelLocation() { return uniformModel; }
GLuint Shader::GetAmbientColourLocation() { return uniformDirectionalLight.uniformColour; }
GLuint Shader::GetAmbientIntensityLocation() { return uniformDirectionalLight.uniformAmbientIntensity; }
GLuint Shader::GetDiffuseIntensityLocation() { return uniformDirectionalLight.uniformDiffuseIntensity; }
GLuint Shader::GetDirectionLocation() { return uniformDirectionalLight.uniformDirection; }
GLuint Shader::GetSpecularIntensityLocation() { return uniformSpecularIntensity; }
GLuint Shader::GetShininessLocation() { return uniformShininess; }

GLuint Shader::GetEyePositionLocation() { return uniformEyePosition; }

void Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}

void Shader::SetDirectionalLight(DirectionalLight* dLight)
{
    dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,
                     uniformDirectionalLight.uniformColour,
                     uniformDirectionalLight.uniformDiffuseIntensity,
		             uniformDirectionalLight.uniformDirection);
}

Shader::~Shader()
{
    ClearShader();
}