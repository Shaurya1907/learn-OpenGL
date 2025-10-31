#pragma once

#include <glad/glad.h>

class Mesh
{
public:
    Mesh();
    void CreateMesh(GLfloat* vertices, GLuint* indices, unsigned int numOfVertices, unsigned int numOfIndices);
    void RenderMesh();
    void ClearMesh();
    ~Mesh();

private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
};
