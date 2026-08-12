#include "vulkan/vk_frame.hpp"
#include "vulkan/vk_command.hpp"
#include <iostream>

void draw_frame(VkDevice device, VkSwapchainKHR swapchain, VkQueue graphics_queue, VkQueue present_queue, VkCommandBuffer command_buffer, VkRenderPass render_pass, const std::vector<VkFramebuffer>& framebuffers, VkExtent2D extent, VkPipeline pipeline, const SyncObjects& sync, VkBuffer vertex_buffer, uint32_t vertex_count, const std::function<void(VkCommandBuffer)>& extra_draw) {
    vkWaitForFences(device, 1, &sync.in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &sync.in_flight_fence);

    uint32_t image_index = 0;
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, sync.image_available_semaphore, VK_NULL_HANDLE, &image_index);

    vkResetCommandBuffer(command_buffer, 0);
    record_command_buffer(command_buffer, render_pass, framebuffers[image_index], extent, pipeline, vertex_buffer, vertex_count, extra_draw);

    VkSemaphore wait_semaphores[] = { sync.image_available_semaphore };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signal_semaphores[] = { sync.render_finished_semaphores[image_index] };

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(graphics_queue, 1, &submit_info, sync.in_flight_fence) != VK_SUCCESS) {
        std::cerr << "Echec de la soumission du command buffer\n";
        return;
    }

    VkSwapchainKHR swapchains[] = { swapchain };

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    vkQueuePresentKHR(present_queue, &present_info);
}