#pragma once

#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

class Window
{

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;

public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);

	GLFWwindow* getWindow() const { return mainWindow; }

	int Initialise();

	GLfloat getBufferWidth() {
		return bufferWidth;
	}
	GLfloat getBufferHeight() {
		return bufferHeight;
	}

	bool getShouldClose() {
		return glfwWindowShouldClose(mainWindow);
	}

	void swapBuffers() {
		glfwSwapBuffers(mainWindow);
	}

	void pollEvents() {
		glfwPollEvents();
	}

	~Window();
};