#include "vk_context.h"
#include "VkBootstrap.h"
#include "logger.h"
#define GLFW_INCLUDE_VULKAN
#define VK_NO_PROTOTYPES
#include "GLFW/glfw3.h"


void VulkanContext::Init(void* p_window){
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Metropolia-Vk")
    .set_minimum_instance_version(1,0,0)
    .request_validation_layers(true)
    .require_api_version(1,0,0)
    .use_default_debug_messenger()
    .build();

    vkb::Instance vkb_inst = inst_ret.value();
    instance = vkb_inst.instance;
    messenger = vkb_inst.debug_messenger;
    ENGINE_INFO("Vulkan instance created");

    glfwCreateWindowSurface(instance, static_cast<GLFWwindow*>(p_window), nullptr, &surface);

    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto physicalDeviceSelector = selector
            .prefer_gpu_device_type()
            .set_minimum_version(1, 0)
            .set_surface(surface)
            .select();

    //create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDeviceSelector.value() };

    vkb::Device vkbDevice = deviceBuilder.build().value();


    // Get the VkDevice handle used in the rest of a Vulkan application
    device = vkbDevice.device;
    physical_device = physicalDeviceSelector.value().physical_device;

    // use vkbootstrap to get a Graphics queue
    graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();

    graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    ENGINE_INFO("Vulkan Device created");

    VkPhysicalDeviceProperties gpu_properties;
    vkGetPhysicalDeviceProperties(physical_device, &gpu_properties);
    ENGINE_INFO("GPU Vendor:{0}", physicalDeviceSelector.value().properties.deviceName);

}

void VulkanContext::CreateSwapchain(uint32_t width, uint32_t height){
    vkb::SwapchainBuilder swapchainBuilder{physical_device,device,surface };
	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.build()
		.value();

	//store swapchain and its related images
	swapchain = vkbSwapchain.swapchain;
	swapchain_images = vkbSwapchain.get_images().value();
	swapchain_image_views = vkbSwapchain.get_image_views().value();
    swapchain_format= vkbSwapchain.image_format;

	//we get actual resolution of the displayed content
	swapchain_extent = vkbSwapchain.extent;

    ENGINE_INFO("Swapchain created");
    ENGINE_INFO("Swapchain extents: ({0}, {1})", swapchain_extent.width, swapchain_extent.height);
}
