#include "vulkan/vk_sync.hpp"
#include <iostream>

SyncObjects create_sync_objects(VkDevice device, uint32_t swapchain_image_count) {
    SyncObjects sync{};

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    bool ok = vkCreateSemaphore(device, &semaphore_info, nullptr, &sync.image_available_semaphore) == VK_SUCCESS
            && vkCreateFence(device, &fence_info, nullptr, &sync.in_flight_fence) == VK_SUCCESS;

    sync.render_finished_semaphores.resize(swapchain_image_count);
    for (uint32_t i = 0; i < swapchain_image_count; ++i) {
        ok = ok && vkCreateSemaphore(device, &semaphore_info, nullptr, &sync.render_finished_semaphores[i]) == VK_SUCCESS;
    }

    if (!ok) {
        std::cerr << "Echec de la creation des objets de synchronisation\n";
    }

    return sync;
}

void destroy_sync_objects(VkDevice device, const SyncObjects& sync) {
    vkDestroySemaphore(device, sync.image_available_semaphore, nullptr);
    for (VkSemaphore sem : sync.render_finished_semaphores) {
        vkDestroySemaphore(device, sem, nullptr);
    }
    vkDestroyFence(device, sync.in_flight_fence, nullptr);
}
