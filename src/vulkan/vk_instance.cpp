#include "vulkan/vk_instance.hpp"

#include "vulkan/vk_context.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <vector>

bool test_vulkan() {
    uint32_t version = 0;
    VkResult result = vkEnumerateInstanceVersion(&version);

    if (result != VK_SUCCESS) {
        std::cerr << "Impossible de recuperer la version Vulkan\n";
        return false;
    }

    std::cout << "Vulkan API disponible : "
              << VK_API_VERSION_MAJOR(version) << "."
              << VK_API_VERSION_MINOR(version) << "."
              << VK_API_VERSION_PATCH(version) << "\n";

    return true;
}

bool check_validation_layer_support() {
    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available.data());

    for (const char* required : kValidationLayers) {
        bool found = false;
        for (const auto& layer : available) {
            if (std::strcmp(required, layer.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

std::vector<const char*> get_required_extensions() {
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

bool create_instance(VulkanContext& ctx, const char* app_name) {
    if (!check_validation_layer_support()) {
        std::cerr << "Validation layers demandees mais non disponibles\n";
        return false;
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    std::vector<const char*> extensions = get_required_extensions();

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
    create_info.ppEnabledLayerNames = kValidationLayers.data();

    if (vkCreateInstance(&create_info, nullptr, &ctx.instance) != VK_SUCCESS) {
        std::cerr << "Echec de la creation de l'instance Vulkan\n";
        return false;
    }

    return true;
}

void destroy_instance(VulkanContext& ctx) {
    if (ctx.instance != VK_NULL_HANDLE) {
        return;
    }

    vkDestroyInstance(ctx.instance, nullptr);
    ctx.instance = VK_NULL_HANDLE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    std::cerr << "[Validation layer] " << callback_data->pMessage << "\n";
    return VK_FALSE; 
}

bool setup_debug_messenger(VulkanContext& ctx) {
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;

    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(ctx.instance, "vkCreateDebugUtilsMessengerEXT"));

    if (!func || func(ctx.instance, &create_info, nullptr, &ctx.debug_messenger) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du debug messenger\n";
        return false;
    }

    return true;
}

void destroy_debug_messenger(VulkanContext& ctx) {
    if (ctx.debug_messenger != VK_NULL_HANDLE) {
        return;
    }

    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(ctx.instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func) {
        func(ctx.instance, ctx.debug_messenger, nullptr);
    }

    ctx.debug_messenger = VK_NULL_HANDLE;
}