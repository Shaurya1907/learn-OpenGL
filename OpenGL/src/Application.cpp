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

#include "CommonValues.h"

#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Model.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"

const float toRadians = 3.14159265f / 180.0f;

// Use GLuint for uniform locations
GLuint uniformSpecularIntensity = 0;
GLuint uniformShininess = 0;
GLuint uniformEyePosition = 0;
GLuint uniformProjection = 0, uniformView = 0, uniformModel = 0;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Window> windowList;
Shader directionalShadowShader;

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
Texture plainTexture;

// Materials
Material shinyMaterial;
Material dullMaterial;

// Models
Model seahawk;

// Light
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int SCR_WIDTH = 1366, SCR_HEIGHT = 768;
float x, y, z;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

GLfloat seahawkAngle = 0.0f;
float seahawkAngularSpeed = 30.0f; // ↓ Set lower to decrease speed (was ~6 deg/s at 60 FPS with 0.1f/frame)
    
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
        glm::vec3 normal = glm::cross(v1, v2);
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

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
       -10.0f, 0.0f, -10.0f,	0.0f, 0.0f,	    0.0f, 1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, 1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, 1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, 1.0f, 0.0f
    };

    // 8 floats per vertex, 24 vertices total (192 floats)
    unsigned int verticesCount = 192;
    unsigned int indicesCount = 36;

    // Calculate average normals in-place in vertices
    calcAverageNormals(indices, indicesCount, vertices, verticesCount, 8, 5);

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, verticesCount, indicesCount);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, verticesCount, indicesCount);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);
}


void CreateShader()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles("Shaders/shader.vert", "Shaders/shader.frag");
    shaderList.push_back(*shader1);

    directionalShadowShader = Shader();
    directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}

void RenderScene()
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[0]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[1]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    plainTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[2]->RenderMesh();

    seahawkAngle += seahawkAngularSpeed * deltaTime;
    if (seahawkAngle >= 360.0f) {
        seahawkAngle -= 360.0f;
    }

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -seahawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(8.0f, 2.0f, 0.0f));
    model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    seahawk.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
    directionalShadowShader.UseShader();

    glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

    light->getShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.GetModelLocation();
    glm::mat4 lightTransform = light->CalculateLightTransform();
    directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    shaderList[0].UseShader();

    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    glViewport(0, 0, 1366, 768);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, cameras[activeCam].getCameraPosition().x, cameras[activeCam].getCameraPosition().y, cameras[activeCam].getCameraPosition().z);

    shaderList[0].SetDirectionalLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount);
    shaderList[0].SetSpotLights(spotLights, spotLightCount);
    
    glm::mat4 lightTransform = mainLight.CalculateLightTransform();
    shaderList[0].SetDirectionalLightTransform(&lightTransform);
    mainLight.getShadowMap()->Read(GL_TEXTURE1);
    shaderList[0].SetTexture(0);
    shaderList[0].SetDirectionalShadowMap(1);

    glm::vec3 lowerLight = cameras[activeCam].getCameraPosition();
    lowerLight.y -= 0.3f;
    //spotLights[0].SetFlash(lowerLight, cameras[activeCam].getCameraDirection());

    RenderScene();
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

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinyMaterial = Material(1.0f, 256.0f);
    dullMaterial = Material(0.3f, 4.0f);

    seahawk = Model();
    seahawk.LoadModel("Models/Seahawk.obj");

    // Directional light: white, some ambient + diffuse
    mainLight = DirectionalLight(
        2048, 2048,                 // shadow map dimensions
        1.0f, 1.0f, 1.0f,           // color
        0.2f, 0.5f,                 // ambient, diffuse
        0.0f, -15.0f, -10.0f        // direction
    );

    // Point light 0: blue-ish with small ambient, use friendlier attenuation
    pointLights[0] = PointLight(
        0.0f, 0.0f, 1.0f,       // color
        0.05f, 1.0f,            // ambient, diffuse
        4.0f, 2.0f, 0.0f,       // position
        1.0f, 0.09f, 0.032f     // attenuation (constant, linear, quadratic)
    );
    pointLightCount++;
    // Point light 1: green-ish
    pointLights[1] = PointLight(
        0.0f, 1.0f, 0.0f,    // color
        0.05f, 1.0f,         // ambient, diffuse
        -4.0f, 2.0f, 0.0f,   // position
        1.0f, 0.09f, 0.032f  // attenuation (constant, linear, quadratic)2f
    );
    pointLightCount++;

    spotLights[0] = SpotLight(
        1.0f, 1.0f, 1.0f,        // color (white)
        0.0f, 2.0f,              // ambient, diffuse
        0.0f, 0.0f, 0.0f,        // position
        0.0f, -1.0f, 0.0f,       // direction
        1.0f, 0.0f, 0.0f,        // attenuation (constant, linear, quadratic)
        20.0f                    // edge angle in degrees
    );
    spotLightCount++;

    spotLights[1] = SpotLight(
        1.0f, 1.0f, 1.0f,        // color (white)
        0.0f, 1.0f,              // ambient, diffuse
        0.0f, 1.5f, 0.0f,        // position
        -1.0f, -1.0f, 0.0f,      // direction
        1.0f, 0.0f, 0.0f,        // attenuation (constant, linear, quadratic)
        20.0f                    // edge angle in degrees
    );
    spotLightCount++;

    x = 0.0f;
    y = 0.0f;
    z = 3.0f;

    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!mainWindow.getShouldClose()) {

        double currentTime = glfwGetTime();
        deltaTime = static_cast<float>(currentTime - lastFrame);
        lastFrame = static_cast<float>(currentTime);

        // process input
        processInput(mainWindow.getWindow(), deltaTime);
        DirectionalShadowMapPass(&mainLight);

        glm::mat4 view = cameras[activeCam].getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cameras[activeCam].zoom),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
            0.1f, 100.0f);

        // Render pass
        RenderPass(projection, view);

        glUseProgram(0);

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