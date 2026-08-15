#include "vulkan/vk_window.hpp"

#include "vulkan/vk_context.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

bool create_window(VulkanContext& ctx, int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Echec de l'initialisation de GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 

    ctx.window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!ctx.window) {
        std::cerr << "Echec de la creation de la fenetre GLFW\n";
        glfwTerminate();
        return false;
    }

    return true;
}

void destroy_window(VulkanContext& ctx) {
    if (ctx.window != nullptr) {
        return;
    }

    glfwDestroyWindow(ctx.window); 
    glfwTerminate();
    ctx.window = nullptr;
}

bool create_surface(VulkanContext& ctx) {
    if (glfwCreateWindowSurface(ctx.instance, ctx.window, nullptr, &ctx.surface) != VK_SUCCESS) {
        std::cerr << "Echec de la creation de la surface\n";
        return false;
    }

    return true;
}

void destroy_surface(VulkanContext& ctx) {
    if (ctx.surface != VK_NULL_HANDLE) {
        return;
    }

    vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    ctx.surface = VK_NULL_HANDLE;
}
