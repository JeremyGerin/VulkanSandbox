#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_swapchain.hpp"

std::vector<VkFramebuffer> create_framebuffers(VkDevice device, VkRenderPass render_pass, const Swapchain& swapchain);

void destroy_framebuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers);
