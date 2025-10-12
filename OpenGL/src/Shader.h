#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>

#include<fstream>
#include<sstream>
#include<string>
#include<iostream>

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void activate();

	// utility functions
	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);

	// uniform functions
	void setMat4(const std::string& name, glm::mat4 val);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
};

#endif