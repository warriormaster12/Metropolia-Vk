#pragma once

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>

class VulkanContext {
public:
    void Init(void* p_window);
    void CreateSwapchain(uint32_t width, uint32_t height);
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
};
