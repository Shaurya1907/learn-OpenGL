#pragma once

#include <glad/glad.h>

class Mesh
{

public:
	Mesh();
	void CreateMesh(GLfloat * vertices, unsigned int numOfvertices);
	void RenderMesh();
	void ClearMesh();
	~Mesh();

private:
	GLuint VAO, VBO;
};