#include "vulkan/vk_window.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow* create_window(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Echec de l'initialisation de GLFW\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // pour l'instant, on gerera le resize plus tard

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Echec de la creation de la fenetre GLFW\n";
        glfwTerminate();
        return nullptr;
    }

    return window;
}