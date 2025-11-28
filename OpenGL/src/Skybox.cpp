#include "Skybox.h"
#include <iostream>
#include <stb/stb_image.h>

Skybox::Skybox()
{
}

Skybox::Skybox(std::vector<std::string> faceLocations)
{
	// Shader setup
	skyShader = new Shader();
	skyShader->CreateFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();

	skyShader->UseShader();

	GLint skyboxLoc = glGetUniformLocation(skyShader->GetShaderID(), "skybox");
	glUniform1i(skyboxLoc, 0);

	// Texture setup
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, bitDepth;
	stbi_set_flip_vertically_on_load(false);

	// Safe unpack alignment for any channel count/width
	GLint prevUnpackAlign = 4;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (size_t i = 0; i < faceLocations.size(); i++) {
		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);
		if (!texData) {
			std::cout << "Failed to find: " << faceLocations[i] << std::endl;
			continue;
		}

		GLenum format = (bitDepth == 4) ? GL_RGBA : GL_RGB;
		GLenum internalFormat = (bitDepth == 4) ? GL_RGBA8 : GL_RGB8;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
			0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, texData);

		stbi_image_free(texData);
	}

	// Restore unpack alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Mesh setup
	unsigned int skyboxIndices[] = {
		0,1,2, 2,1,3,
		2,3,5, 5,3,7,
		5,7,4, 4,7,6,
		4,6,0, 0,6,1,
		4,0,5, 5,0,2,
		1,6,3, 3,6,7
	};

	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,  0,0,  0,0,0,
		-1.0f, -1.0f, -1.0f,  0,0,  0,0,0,
		 1.0f,  1.0f, -1.0f,  0,0,  0,0,0,
		 1.0f, -1.0f, -1.0f,  0,0,  0,0,0,
		-1.0f,  1.0f,  1.0f,  0,0,  0,0,0,
		 1.0f,  1.0f,  1.0f,  0,0,  0,0,0,
		-1.0f, -1.0f,  1.0f,  0,0,  0,0,0,
		 1.0f, -1.0f,  1.0f,  0,0,  0,0,0
	};

	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);
}

void Skybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL); // ensure skybox passes at depth = 1.0

	skyShader->UseShader();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	skyShader->Validate();
	skyMesh->RenderMesh();

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}

Skybox::~Skybox()
{
}