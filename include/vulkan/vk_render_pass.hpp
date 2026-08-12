#pragma once

#include <vulkan/vulkan.h>

VkRenderPass create_render_pass(VkDevice device, VkFormat swapchain_format);
