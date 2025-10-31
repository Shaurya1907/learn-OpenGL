#include "Texture.h"
#include <iostream>

Texture::Texture()
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = "";
}

Texture::Texture(const std::string& fileLoc)
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = fileLoc;
}

void Texture::LoadTexture()
{
    unsigned char* texData = stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0);
    if (!texData) {
        std::cout << "Failed to find: " << fileLocation << std::endl;
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Use trilinear filtering with mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (bitDepth == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(texData);
}

void Texture::UseTexture(GLenum texUnit)
{
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture()
{
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = "";
}

Texture::~Texture()
{
    ClearTexture();
}