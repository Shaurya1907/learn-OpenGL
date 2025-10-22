#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb/stb_image.h>
#include<vector>

#include<fstream>
#include<sstream>
#include<string>
#include<streambuf>

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"Window.h"
#include"Shader.h"
#include"Mesh.h"

#include"io/keyboard.h"
#include"io/mouse.h"
#include"io/Joystick.h"
#include"io/camera.h"

std::vector<Mesh*> meshList;
std::vector<Window>windowList;

glm::mat4 mouseTransform = glm::mat4(1.0f);

Camera cameras[2] = {
    Camera(glm::vec3(0.0f, 0.0f, 3.0f)),
    Camera(glm::vec3(10.0f, 10.0f, 10.0f))
};

int activeCam = 0;

glm::mat4 transform = glm::mat4(1.0f);
Joystick mainJ(0);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
    
    //move camera
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

int main() {
    int success;
    char infoLog[512];

	Window mainWindow(800, 600);
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

	Shader shader("assets/vertex_core.glsl", "assets/fragment_core1.glsl");

    float vertices[] = {
    //Position        
    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  

    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f 
    };

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, sizeof(vertices) / sizeof(vertices[0]));
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, sizeof(vertices) / sizeof(vertices[0]));
    meshList.push_back(obj2);

	shader.activate();  

    x = 0.0F;
	y = 0.0F;
	z = 3.0F;

	mainJ.update();
    if (mainJ.isPresent()) {
        std::cout << mainJ.getName() << "is present." << std::endl;
    }
    else {
		std::cout << "Not present." << std::endl;
    }

    // Render loop
    while (!mainWindow.getShouldClose()) {

        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        // process input
        processInput(mainWindow.getWindow(), deltaTime);

        // render
        glClearColor(0.4, 0.4, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();

        //create transformation for screen
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

		//view = glm::translate(view, glm::vec3(-x, -y, -z));
        view = cameras[activeCam].getViewMatrix();
        projection = glm::perspective(glm::radians(cameras[activeCam].zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

        //--- First CUBE at origin
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        meshList[0]->RenderMesh();

        //--- Second CUBE translated+X
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        model2 = glm::rotate(model2, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f)); // Rotating around Y axis
        shader.setMat4("model", model2);
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