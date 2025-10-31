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
    uniformAmbientIntensity = -1;
    uniformAmbientColour = -1;
    uniformDiffuseIntensity = -1;
    uniformDirection = -1;
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
    uniformAmbientColour = glGetUniformLocation(shaderID, "directionalLight.colour");
    uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.ambientIntensity");
    uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.diffuseIntensity");
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

GLint Shader::GetProjectionLocation() { return uniformProjection; }
GLint Shader::GetViewLocation() { return uniformView; }
GLint Shader::GetModelLocation() { return uniformModel; }
GLint Shader::GetAmbientColourLocation() { return uniformAmbientColour; }
GLint Shader::GetAmbientIntensityLocation() { return uniformAmbientIntensity; }
GLint Shader::GetDiffuseIntensityLocation() { return uniformDiffuseIntensity; }
GLint Shader::GetDirectionLocation() { return uniformDirection; }

void Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}

Shader::~Shader()
{
    ClearShader();
}