#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

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

    // Utility functions
    std::string ReadFile(const char* fileLocation);

    // Get uniform locations for direct OpenGL use
    GLint GetProjectionLocation();
    GLint GetViewLocation();
    GLint GetModelLocation();

    GLint GetAmbientIntensityLocation();
    GLint GetAmbientColourLocation();
    GLint GetDiffuseIntensityLocation();
    GLint GetDirectionLocation();

private:
    GLuint shaderID;
    GLint uniformProjection;
    GLint uniformView;
    GLint uniformModel;

    GLint uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection;

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

#endif