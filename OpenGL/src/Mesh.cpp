#include "Mesh.h"

Mesh::Mesh() {
    VAO = 0;
    VBO = 0;
    EBO = 0;
    indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, GLuint* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
    indexCount = numOfIndices;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex Buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numOfVertices, vertices, GL_STATIC_DRAW);

    // Element Buffer (Indices)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numOfIndices, indices, GL_STATIC_DRAW);

    // Vertex attribute pointer setup:
    // Position (layout location 0): 3 floats, offset 0 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TexCoord (location 1): 2 floats, offset 3 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal (location 2): 3 floats, offset 5 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::RenderMesh() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::ClearMesh() {
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

Mesh::~Mesh() {
    ClearMesh();
}
