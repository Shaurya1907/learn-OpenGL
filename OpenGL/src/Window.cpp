#include "Window.h"

Window::Window() : width(800), height(600), mainWindow(nullptr), bufferWidth(0), bufferHeight(0) {
}

Window::Window(GLint windowWidth, GLint windowHeight)
    : width(windowWidth), height(windowHeight), mainWindow(nullptr), bufferWidth(0), bufferHeight(0) {
}
int Window::Initialise() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mainWindow = glfwCreateWindow(width, height, "OpenGL Window", nullptr, nullptr);
    if (!mainWindow) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mainWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
    glViewport(0, 0, bufferWidth, bufferHeight);

    return 0;
}

Window::~Window() {
    if (mainWindow) {
        glfwDestroyWindow(mainWindow);
    }
	glfwTerminate();
}