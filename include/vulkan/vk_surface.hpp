#pragma once

#include <vulkan/vulkan.h>

struct GLFWwindow;
VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow* window);