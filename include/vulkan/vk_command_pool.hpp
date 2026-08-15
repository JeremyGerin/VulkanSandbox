#pragma once

#include <vulkan/vulkan.h>
#include "vk_context.hpp"

bool create_command_pool(VulkanContext& ctx);
void destroy_command_pool(VulkanContext& ctx);