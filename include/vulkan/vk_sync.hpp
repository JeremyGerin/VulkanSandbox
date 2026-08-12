#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct SyncObjects {
    VkSemaphore image_available_semaphore;
    std::vector<VkSemaphore> render_finished_semaphores;
    VkFence in_flight_fence;
};

SyncObjects create_sync_objects(VkDevice device, uint32_t swapchain_image_count);

void destroy_sync_objects(VkDevice device, const SyncObjects& sync);
