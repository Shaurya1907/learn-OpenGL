#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb/stb_image.h>
#include <vector>

#include <fstream>
#include <sstream>
#include <string>
#include <streambuf>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/Joystick.h"
#include "io/camera.h"

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Window> windowList;

glm::mat4 mouseTransform = glm::mat4(1.0f);

// Cameras
Camera cameras[2] = {
    Camera(glm::vec3(0.0f, 0.0f, 3.0f)),
    Camera(glm::vec3(10.0f, 10.0f, 10.0f))
};

// Struct to hold vertex info (extend if needed)
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec2 texCoord;
};

int activeCam = 0;

// Textures
Texture brickTexture;
Texture dirtTexture;

// Materials
Material shinyMaterial;
Material dullMaterial;

// Light
Light mainLight;

glm::mat4 transform = glm::mat4(1.0f);
Joystick mainJ(0);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;
float x, y, z;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow* mainWindow, double dt)
{
    if (Keyboard::key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(mainWindow, true);

    if (Keyboard::keyWentDown(GLFW_KEY_TAB)) {
        activeCam += (activeCam == 0) ? 1 : -1;
    }

    // Move camera
    if (Keyboard::key(GLFW_KEY_W)) {
        cameras[activeCam].updateCameraPos(CameraDirection::FORWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        cameras[activeCam].updateCameraPos(CameraDirection::BACKWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        cameras[activeCam].updateCameraPos(CameraDirection::RIGHT, dt);
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        cameras[activeCam].updateCameraPos(CameraDirection::LEFT, dt);
    }
    if (Keyboard::key(GLFW_KEY_SPACE)) {
        cameras[activeCam].updateCameraPos(CameraDirection::UP, dt);
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
        cameras[activeCam].updateCameraPos(CameraDirection::DOWN, dt);
    }
    mainJ.update();

    double dx = Mouse::getDX(), dy = Mouse::getDY();
    if (dx != 0 || dy != 0) {
        cameras[activeCam].updateCameraDirection(dx, dy);
    }

    double scrollDy = Mouse::getScrollY();
    if (scrollDy != 0) {
        cameras[activeCam].updateCameraZoom(scrollDy);
    }
}

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
    unsigned int vLength, unsigned int normalOffset)
{
    // Reset all normals to zero
    for (size_t i = 0; i < verticeCount; i += vLength) {
        vertices[i + normalOffset] = 0.0f;
        vertices[i + normalOffset + 1] = 0.0f;
        vertices[i + normalOffset + 2] = 0.0f;
    }

    // Calculate face normals and accumulate them for each vertex of the triangle
    for (size_t i = 0; i < indiceCount; i += 3) {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v2, v1);
        normal = glm::normalize(normal);

        vertices[in0 + normalOffset] += normal.x;
        vertices[in0 + normalOffset + 1] += normal.y;
        vertices[in0 + normalOffset + 2] += normal.z;

        vertices[in1 + normalOffset] += normal.x;
        vertices[in1 + normalOffset + 1] += normal.y;
        vertices[in1 + normalOffset + 2] += normal.z;

        vertices[in2 + normalOffset] += normal.x;
        vertices[in2 + normalOffset + 1] += normal.y;
        vertices[in2 + normalOffset + 2] += normal.z;
    }

    // Normalize all accumulated normals
    for (size_t i = 0; i < verticeCount; i += vLength) {
        glm::vec3 vec(vertices[i + normalOffset], vertices[i + normalOffset + 1], vertices[i + normalOffset + 2]);
        vec = glm::normalize(vec);
        vertices[i + normalOffset] = vec.x;
        vertices[i + normalOffset + 1] = vec.y;
        vertices[i + normalOffset + 2] = vec.z;
    }
}

void CreateObject() {
    unsigned int indices[] = {
        4, 5, 6, 6, 7, 4,   // Front face
        0, 3, 2, 2, 1, 0,   // Back face
        0, 4, 7, 7, 3, 0,   // Left face
        1, 2, 6, 6, 5, 1,   // Right face
        0, 1, 5, 5, 4, 0,   // Bottom face
        3, 7, 6, 6, 2, 3    // Top face
    };

    GLfloat vertices[] = {
        // x      y      z       u     v   nx ny nz (all normals zero)
        // Front
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  0,0,0,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0,0,0,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  0,0,0,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  0,0,0,

        // Back
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  0,0,0,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  0,0,0,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  0,0,0,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  0,0,0,

        // Left
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  0,0,0,
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0,0,0,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  0,0,0,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  0,0,0,

        // Right
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  0,0,0,
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  0,0,0,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  0,0,0,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  0,0,0,

         // Bottom - paste image exactly once
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  0,0,0,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  0,0,0,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  0,0,0,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,  0,0,0,

         // Top - paste image exactly once
         -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  0,0,0,
          0.5f,  0.5f, -0.5f,   1.0f, 0.0f,  0,0,0,
          0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  0,0,0,
         -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  0,0,0
    };

    // 8 floats per vertex, 24 vertices total (192 floats)
    unsigned int verticesCount = 192;
    unsigned int indicesCount = 36;

    // Calculate average normals in-place in vertices
    calcAverageNormals(indices, indicesCount, vertices, verticesCount, 8, 5);

    Mesh* cube1 = new Mesh();
    cube1->CreateMesh(vertices, indices, verticesCount, indicesCount);
    meshList.push_back(cube1);

    Mesh* cube2 = new Mesh();
    cube2->CreateMesh(vertices, indices, verticesCount, indicesCount);
    meshList.push_back(cube2);
}


void CreateShader()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles("assets/vertex_core.glsl", "assets/fragment_core1.glsl");
    shaderList.push_back(*shader1);
}

int main() {
    int success;
    char infoLog[512];

    Window mainWindow(SCR_WIDTH, SCR_HEIGHT);
    if (mainWindow.Initialise() != 0) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(mainWindow.getWindow(), framebuffer_size_callback);

    // keyboard input
    glfwSetKeyCallback(mainWindow.getWindow(), Keyboard::keyCallback);

    // mouse input
    glfwSetCursorPosCallback(mainWindow.getWindow(), Mouse::cursorPosCallback);
    glfwSetMouseButtonCallback(mainWindow.getWindow(), Mouse::mouseButtonCallback);
    glfwSetScrollCallback(mainWindow.getWindow(), Mouse::scrollCallback);

    glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    CreateShader();
    CreateObject();

    GLuint uniformProjection = 0, uniformView = 0, uniformModel = 0;

    brickTexture = Texture("assets/Textures/brick.png");
    brickTexture.LoadTexture();
    dirtTexture = Texture("assets/Textures/brick.png");
    dirtTexture.LoadTexture();

	shinyMaterial = Material(1.0f, 32.0f);
	dullMaterial = Material(0.3f, 4.0f);

    mainLight = Light(1.0f, 1.0f, 1.0f, 0.2f,
        0.0f, 1.0f, 1.0f, 0.8f);

    // Use GLuint for uniform locations
    GLuint uniformAmbientIntensity = 0;
    GLuint uniformAmbientColour = 0;
    GLuint uniformDiffuseIntensity = 0;
    GLuint uniformDirection = 0;
	GLuint uniformSpecularIntensity = 0;
	GLuint uniformShininess = 0;

	GLuint uniformEyePosition = 0;


    x = 0.0f;
    y = 0.0f;
    z = 3.0f;

    mainJ.update();
    if (mainJ.isPresent()) {
        std::cout << mainJ.getName() << " is present." << std::endl;
    }
    else {
        std::cout << "Not present." << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
  

    // Render loop
    while (!mainWindow.getShouldClose()) {

        double currentTime = glfwGetTime();
        deltaTime = static_cast<float>(currentTime - lastFrame);
        lastFrame = static_cast<float>(currentTime);

        // process input
        processInput(mainWindow.getWindow(), deltaTime);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader& shader = shaderList[0];
        shader.UseShader();

        glm::mat4 view = cameras[activeCam].getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cameras[activeCam].zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniform3f(uniformEyePosition,
            cameras[activeCam].cameraPos.x,
            cameras[activeCam].cameraPos.y,
			cameras[activeCam].cameraPos.z);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        uniformAmbientIntensity = shader.GetAmbientIntensityLocation();
        uniformAmbientColour = shader.GetAmbientColourLocation();
        uniformDiffuseIntensity = shader.GetDiffuseIntensityLocation();
        uniformDirection = shader.GetDirectionLocation();

		uniformEyePosition = shader.GetEyePositionLocation();

		uniformSpecularIntensity = shader.GetSpecularIntensityLocation();
		uniformShininess = shader.GetShininessLocation();

        mainLight.UseLight(
            uniformAmbientIntensity,
            uniformAmbientColour,
            uniformDiffuseIntensity,
            uniformDirection
        );

        //--- First CUBE at origin
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setInt("theTexture", 0);
        brickTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[0]->RenderMesh();

        //--- Second CUBE translated+X
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
        model2 = glm::rotate(model2, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model2);
        shader.setInt("theTexture", 0);
        dirtTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[1]->RenderMesh();

		//--- Third CUBE translated -X
        glm::mat4 model3 = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));
        model3 = glm::rotate(model3, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model3);
        shader.setInt("theTexture", 0);
        dirtTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[1]->RenderMesh();

        glBindVertexArray(0);

        mainWindow.swapBuffers();
        mainWindow.pollEvents();
    }

    // Cleanup BOTH meshes before exit
    for (auto mesh : meshList) {
        if (mesh) {
            mesh->ClearMesh();
            delete mesh;
        }
    }
    meshList.clear();

    return 0;
}