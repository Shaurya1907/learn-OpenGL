#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stb/stb_image.h>


#include<fstream>
#include<sstream>
#include<string>
#include<streambuf>

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"Shader.h"

#include"io/keyboard.h"
#include"io/mouse.h"
#include"io/Joystick.h"
#include"io/camera.h"

float mixVal = 0.5f;

glm::mat4 mouseTransform = glm::mat4(1.0f);

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

void processInput(GLFWwindow* window, double dt)
{
    if (Keyboard::key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);

    // change mix value
    if (Keyboard::keyWentDown(GLFW_KEY_UP)) {
        mixVal += 0.5f;
        if (mixVal >= 1.0f) {
            mixVal = 1.0f;
        }
    }
    if (Keyboard::keyWentDown(GLFW_KEY_DOWN)) {
        mixVal -= 0.5f;
        if (mixVal < 0.0f) {
            mixVal = 0.0f;
        }
    }
    
    //move camera
    if (Keyboard::key(GLFW_KEY_W)) {
        camera.updateCameraPos(CameraDirection::FORWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        camera.updateCameraPos(CameraDirection::BACKWARD, dt);
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        camera.updateCameraPos(CameraDirection::RIGHT, dt);
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        camera.updateCameraPos(CameraDirection::LEFT, dt);
    }
    if (Keyboard::key(GLFW_KEY_SPACE)) {
        camera.updateCameraPos(CameraDirection::UP, dt);
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
        camera.updateCameraPos(CameraDirection::DOWN, dt);
    }
	mainJ.update();

    double dx = Mouse::getDX(), dy = Mouse::getDY();
    if (dx != 0 || dy != 0) {
        camera.updateCameraDirection(dx, dy);
    }

    double scrollDy = Mouse::getScrollY();
    if (scrollDy != 0) {
        camera.updateCameraZoom(scrollDy);
    }
}

int main() {
    int success;
    char infoLog[512];

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // keyboard input
    glfwSetKeyCallback(window, Keyboard::keyCallback);

	// mouse input
    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	Shader shader("assets/vertex_core.glsl", "assets/fragment_core1.glsl");

    float vertices[] = {
    //Position        //texture coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // VAO, VBO setup
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

    // textures
	unsigned int texture, texture1;

	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // load image
	int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/image.png", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    data = stbi_load("assets/image1.png", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

	shader.activate();  
	shader.setInt("ourTexture", 0);
	shader.setInt("ourTexture1", 1);

    /*glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.activate();
    shader.setMat4("transform", trans);*/

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
    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        // process input
        processInput(window, deltaTime);

        // render
        glClearColor(0.4, 0.4, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		//trans = glm::rotate(trans, glm::radians((float)glfwGetTime() / 1000.0f), glm::vec3(0.0f, 0.0f, 0.1f));
        //shader.setMat4("transform", trans);

        glBindVertexArray(VAO);

        //create transformation for screen
		glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-55.0f), glm::vec3(0.5f));
		//view = glm::translate(view, glm::vec3(-x, -y, -z));
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.activate();

		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

        shader.setFloat("mixVal", mixVal);
        
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glm::mat4 model = trans * transform;
        //shader.setMat4("transform", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
   
    glfwTerminate();
    return 0;
}