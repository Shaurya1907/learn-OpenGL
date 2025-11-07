#pragma once

#include<glad/glad.h>
#include <stb/stb_image.h>
#include<string>

class Texture
{
public:
	Texture();
	Texture(const std::string& fileLoc);

	bool LoadTexture();
	bool LoadTextureA();

	void UseTexture(GLenum texUnit = GL_TEXTURE0);
	void ClearTexture();

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	std::string fileLocation;
};
