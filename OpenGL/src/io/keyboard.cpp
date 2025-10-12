#include "keyboard.h"

bool Keyboard::keys[GLFW_KEY_LAST + 1] = { 0 };
bool Keyboard::keysChanged[GLFW_KEY_LAST + 1] = { 0 };

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key > GLFW_KEY_LAST) return; // bounds check

    if (action == GLFW_PRESS) {
        if (!keys[key]) {
            keys[key] = true;
            keysChanged[key] = true;
        }
    } else if (action == GLFW_RELEASE) {
        if (keys[key]) {
            keys[key] = false;
            keysChanged[key] = true;
        }
    }
    // Do nothing for GLFW_REPEAT
}

bool Keyboard::key(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return keys[key];
}

bool Keyboard::keyChanged(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    bool ret = keysChanged[key];
    keysChanged[key] = false;
    return ret;
}

bool Keyboard::keyWentUp(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return !keys[key] && keyChanged(key);
}

bool Keyboard::keyWentDown(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return keys[key] && keyChanged(key);
}