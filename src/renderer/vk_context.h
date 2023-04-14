#pragma once

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>

struct FrameData{
    VkCommandPool main_pool;
    VkCommandBuffer main_buffer;

    VkFence render_fence;
    VkSemaphore render_semaphore;
    VkSemaphore present_semaphore;
};

class VulkanContext {
public:
    void Init(void* p_window);
    void CreateSwapchain(uint32_t width, uint32_t height);
    void InitCommandBuffers();
    void InitRenderpass();
    void DrawsObjects();
    void Cleanup();
private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    VkSurfaceKHR surface;

    VkDevice device;
    VkPhysicalDevice physical_device;

    VkQueue graphics_queue;
    uint32_t graphics_queue_family;

    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkFormat swapchain_format;

    VkExtent2D swapchain_extent;

    FrameData frames[2];

    void PrepareFrame();
    void SubmitFrame();

    uint32_t frame_count = 0;
    uint32_t image_index = 0;

    VkRenderPass main_pass;
    std::vector<VkFramebuffer> main_framebuffer;
};
