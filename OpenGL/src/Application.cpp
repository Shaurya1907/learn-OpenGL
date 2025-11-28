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
#include "Skybox.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"

const float toRadians = 3.14159265f / 180.0f;

// Use GLuint for uniform locations
GLuint uniformSpecularIntensity = 0;
GLuint uniformShininess = 0;
GLuint uniformEyePosition = 0;
GLuint uniformProjection = 0, uniformView = 0, uniformModel = 0;
GLuint uniformOmniLightPos = 0;
GLuint uniformFarPlane = 0;

GLuint uniformLightView = 0;
GLuint uniformLightProjection = 0;
bool showLightView = true;  // Toggle with V key

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Window> windowList;
Shader directionalShadowShader;
Shader omniShadowShader;

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
Model AirPlane;
Model Old_Water_Tower;
Model Barn;

// Light
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Skybox
Skybox skybox;

unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float sunAngle = 0.0f;
float sunSpeed = 1.0f;

unsigned int SCR_WIDTH = 1280, SCR_HEIGHT = 1024;
float x, y, z;

// WireFrame
bool wireframeMode = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

GLfloat seahawkAngle = 0.0f;
float seahawkAngularSpeed = 10.0f; // Set lower to decrease speed (was ~6 deg/s at 60 FPS with 0.1f/frame)
    
void processInput(GLFWwindow* mainWindow, double dt)
{
    if (Keyboard::key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(mainWindow, true);

    if (Keyboard::keyWentDown(GLFW_KEY_TAB)) {
        activeCam += (activeCam == 0) ? 1 : -1;
    }

    if (Keyboard::keyWentDown(GLFW_KEY_L)) {
        spotLights[0].Toggle();
    }
    if (Keyboard::keyWentDown(GLFW_KEY_T))
    {
        wireframeMode = !wireframeMode;
    }

	// Light view toggle
    if (Keyboard::keyWentDown(GLFW_KEY_V)) {
        showLightView = !showLightView;
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

    directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
    omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");

    // Initialize light viewport uniforms
    uniformLightView = shaderList[0].GetViewLocation();
    uniformLightProjection = shaderList[0].GetProjectionLocation();
}


void RenderScene()
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    //meshList[0]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	//model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    //meshList[1]->RenderMesh();

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
    model = glm::translate(model, glm::vec3(15.0f, 1.0f, 0.0f));
    model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    seahawk.RenderModel(wireframeMode);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    seahawk.RenderModel(wireframeMode);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -seahawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-50.0f, 5.0f, 0.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, 35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    AirPlane.RenderModel(wireframeMode);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    Old_Water_Tower.RenderModel(wireframeMode);
}

void DirectionalShadowMapPass(DirectionalLight* light, float angle)
{
    directionalShadowShader.UseShader();

    glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());
    light->getShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    // IMPORTANT: set the global uniformModel to the depth shader's model location
    uniformModel = directionalShadowShader.GetModelLocation();

    // Use the same light transform as the main pass
    glm::mat4 lightTransform = light->CalculateLightTransform(angle);
    directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

    directionalShadowShader.Validate();

    // Optional: reduce self-shadowing (acne) by rendering front faces into the depth map
    GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
    if (!wasCullEnabled) glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Render ALL casters into the shadow map
    RenderScene();

    // Restore cull state
    glCullFace(GL_BACK);
    if (!wasCullEnabled) glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void OmniShadowMapPass(PointLight* light)
{
    omniShadowShader.UseShader();

    glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

    light->getShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
    uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
    uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

    glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
    glUniform1f(uniformFarPlane, light->GetFarPlane());
    omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

    omniShadowShader.Validate();

    RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderLightViewport()
{
    if (!showLightView) return;

    // Mini viewport rectangle
    const GLint vpX = static_cast<GLint>(SCR_WIDTH) - 320;
    const GLint vpY = 5;
    const GLsizei vpW = 300;
    const GLsizei vpH = 300;

    // Limit clear to the mini-viewport
    glViewport(vpX, vpY, vpW, vpH);
    glEnable(GL_SCISSOR_TEST);
    glScissor(vpX, vpY, vpW, vpH);
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f); // subtle background to make it visible
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // Use main scene shader for colorful output
    shaderList[0].UseShader();

    const GLuint modelLoc = shaderList[0].GetModelLocation();
    const GLuint projLoc  = shaderList[0].GetProjectionLocation();
    const GLuint viewLoc  = shaderList[0].GetViewLocation();
    const GLuint eyeLoc = shaderList[0].GetEyePositionLocation();
    const GLuint specLoc = shaderList[0].GetSpecularIntensityLocation();
    const GLuint shineLoc = shaderList[0].GetShininessLocation();

    // Construct a reasonable light camera for viewing
    // Eye is opposite to light direction so we "look along" the light
    const glm::vec3 lightDir = mainLight.GetDirection(); // ensure DirectionalLight exposes GetDirection()
    const glm::vec3 eye      = -lightDir * 30.0f;        // pull back along direction
    const glm::vec3 target   = glm::vec3(0.0f);
    const glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);

    const glm::mat4 lightView = glm::lookAt(eye, target, up);

    // If the light exposes its orthographic projection, prefer it; else use a small perspective.
    // Replace with mainLight.GetLightProjection() if available.
    const glm::mat4 lightProj = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 100.0f);

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(lightProj));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(lightView));

    glUniform3f(eyeLoc, eye.x, eye.y, eye.z);  // Light eye position
    glUniform1f(specLoc, 1.0f);                // Specular intensity
    glUniform1f(shineLoc, 256.0f);             // Shininess

    // Lights/textures as normal
    shaderList[0].SetDirectionalLight(&mainLight);
    mainLight.getShadowMap()->Read(GL_TEXTURE2);
    shaderList[0].SetDirectionalShadowMap(2);
    shaderList[0].SetTexture(1);

    shaderList[0].Validate();

    // Render the scene into the mini viewport
    RenderScene();

    glUseProgram(0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, float sunAngle)
{
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

    shaderList[0].UseShader();

    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, cameras[activeCam].getCameraPosition().x, cameras[activeCam].getCameraPosition().y, cameras[activeCam].getCameraPosition().z);

    shaderList[0].SetDirectionalLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
    shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
    
    glm::mat4 lightTransform = mainLight.CalculateLightTransform(sunAngle);
    shaderList[0].SetDirectionalLightTransform(&lightTransform);
    mainLight.getShadowMap()->Read(GL_TEXTURE2);
    shaderList[0].SetTexture(1);
    shaderList[0].SetDirectionalShadowMap(2);

    glm::vec3 lowerLight = cameras[activeCam].getCameraPosition();
    lowerLight.y -= 0.3f;
    spotLights[0].SetFlash(lowerLight, cameras[activeCam].getCameraDirection());

    shaderList[0].Validate();

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

	AirPlane = Model();
	AirPlane.LoadModel("Models/Airplane.obj");

    Old_Water_Tower = Model();
    Old_Water_Tower.LoadModel("Models/old_water_tower_OBJ.obj");

    // Directional light: white, some ambient + diffuse
    mainLight = DirectionalLight(
        4096, 4096,                  // shadow map dimensions
        1.0f, 1.0f, 1.0f,            // color
        0.1f, 0.8f,                  // ambient, diffuse
        0.0f, -15.0f, -10.0f         // direction
    );

    // Point light 0: blue-ish with small ambient, use friendlier attenuation
    pointLights[0] = PointLight(
		1024, 1024,                  // shadow map dimensions
        0.1f, 100.0f,                // near, far planes
        0.0f, 1.0f, 0.0f,            // color
        0.0f, 0.4f,                  // ambient, diffuse
        -2.0f, 3.0f, 0.0f,           // position
        0.3f, 0.02f, 0.01f           // attenuation (constant, linear, quadratic)
    );
    pointLightCount++;
    // Point light 1: green-ish
    pointLights[1] = PointLight(
		1024, 1024,                 // shadow map dimensions
        0.1f, 100.0f,               // near, far planes
        0.0f, 0.0f, 1.0f,           // color
        0.0f, 0.4f,                 // ambient, diffuse
        2.0f, 3.0f, 0.0f,           // position
        0.3f, 0.002f, 0.001f        // attenuation (constant, linear, quadratic)2f
    );
    pointLightCount++;

    spotLights[0] = SpotLight(
        4096, 4096,              // shadow map dimensions
        0.01, 100.0f,            // near, far planes
        1.0f, 1.0f, 1.0f,        // color (white)
        0.0f, 1.0f,              // ambient, diffuse
        0.0f, 0.0f, 0.0f,        // position
        0.0f, -1.0f, 0.0f,       // direction
        1.0f, 0.0f, 0.0f,        // attenuation (constant, linear, quadratic)
        20.0f                    // edge angle in degrees
    );
    spotLightCount++;

    spotLights[1] = SpotLight(
        1024, 1024,              // shadow map dimensions
        0.01, 100.0f,            // near, far planes
        1.0f, 1.0f, 1.0f,        // color (white)
        0.0f, 1.0f,              // ambient, diffuse
        0.0f, 1.5f, 0.0f,        // position
        -1.0f, -1.0f, 0.0f,      // direction
        1.0f, 0.0f, 0.0f,        // attenuation (constant, linear, quadratic)
        20.0f                    // edge angle in degrees
    );
    //spotLightCount++;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/interstellar_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/interstellar_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/interstellar_up.tga");
	skyboxFaces.push_back("Textures/Skybox/interstellar_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/interstellar_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/interstellar_ft.tga");

	skybox = Skybox(skyboxFaces);

    x = 0.0f;
    y = 0.0f;
    z = 3.0f;

    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!mainWindow.getShouldClose()) {
        double currentTime = glfwGetTime();
        deltaTime = static_cast<float>(currentTime - lastFrame);
        lastFrame = static_cast<float>(currentTime);

        // Update sun
        sunAngle += sunSpeed * deltaTime;
        if (sunAngle > 6.28318f) sunAngle -= 6.28318f;

        float radius = 25.0f;
        float fixedY = 15.0f;
        float xPos = radius * cos(sunAngle);
        float zPos = radius * sin(sunAngle);
        glm::vec3 lightPos(xPos, fixedY, zPos);
        glm::vec3 lightDir = glm::normalize(-lightPos);
        mainLight.SetDirection(lightDir);

        processInput(mainWindow.getWindow(), deltaTime);

        // 1. Shadow passes FIRST
        DirectionalShadowMapPass(&mainLight, sunAngle);
        for (size_t i = 0; i < pointLightCount; i++) {
            OmniShadowMapPass(&pointLights[i]);
        }
        for (size_t i = 0; i < spotLightCount; i++) {
            OmniShadowMapPass(&spotLights[i]);
        }

        // 2. MAIN SCENE - clears entire screen ONCE
        glm::mat4 view = cameras[activeCam].getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cameras[activeCam].zoom),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        RenderPass(projection, view, sunAngle);

        // 3. LIGHT VIEWPORT - NO FULL CLEAR, only depth
        glm::mat4 lightTransform = mainLight.CalculateLightTransform(sunAngle);
        RenderLightViewport();  // Fixed call

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