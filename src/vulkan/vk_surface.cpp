#include "vulkan/vk_surface.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow* window) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        std::cerr << "Echec de la creation de la surface\n";
        return VK_NULL_HANDLE;
    }

    return surface;
}