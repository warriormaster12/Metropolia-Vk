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


void VulkanContext::InitCommandBuffers(){
    /*for sending draw commands to the gpu, we need to record these commands first to the command buffer.
    Before we can record commands, we need to create command pool and allocate a command buffer with that pool*/

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = graphics_queue_family;
    pool_info.pNext = nullptr;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    for(int i = 0; i < 2; i++){
        VkResult result = vkCreateCommandPool(device, &pool_info, nullptr, &frames[i].main_pool);
        if(result != VK_SUCCESS){
            ENGINE_ERROR("failed to create command pool");
        }

        VkCommandBufferAllocateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_info.commandPool = frames[i].main_pool;
        buffer_info.commandBufferCount = 1;
        buffer_info.pNext = nullptr;
        buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        result = vkAllocateCommandBuffers(device, &buffer_info, &frames[i].main_buffer);
        if(result != VK_SUCCESS){
            ENGINE_ERROR("failed to allocate command buffer");
        }
    }

    ENGINE_INFO("Allocated command buffers");
    /*
    Fences and Semaphores are Vulkan sync objects
    Fences are for synchronizing between cpu and gpu
    Semaphores are meant for synchronizing between gpu and gpu operations
    In this case we are creating two semaphores for rendering a swapchain image and then presenting it.
    */
    VkFenceCreateInfo fence_info = {};
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.pNext = nullptr;
    semaphore_info.flags = 0;

    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.pNext = nullptr;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for(int i = 0; i < 2; i++){
        VkResult result = vkCreateFence(device, &fence_info, nullptr, &frames[i].render_fence);
        if(result != VK_SUCCESS){
            ENGINE_ERROR("failed to create render fence");
        }

        result = vkCreateSemaphore(device, &semaphore_info, nullptr, &frames[i].render_semaphore);
        if(result != VK_SUCCESS){
            ENGINE_ERROR("failed to create render semaphore");
        }
        result = vkCreateSemaphore(device, &semaphore_info, nullptr, &frames[i].present_semaphore);
        if(result != VK_SUCCESS){
            ENGINE_ERROR("failed to create present semaphore");
        }
    }
    ENGINE_INFO("Sync objects created");
}

void VulkanContext::InitRenderpass(){
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = swapchain_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pNext = nullptr;

    info.pAttachments = &color_attachment;
    info.attachmentCount = 1;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;

    VkResult result = vkCreateRenderPass(device, &info, nullptr,&main_pass);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("failed to create renderpass");
    }

    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;
    fb_info.attachmentCount = 1;
    fb_info.height = swapchain_extent.height;
    fb_info.width = swapchain_extent.width;
    fb_info.renderPass = main_pass;
    fb_info.layers = 1;

    for(size_t i = 0; i < swapchain_image_views.size(); i++){
        VkFramebuffer buffer;
        fb_info.pAttachments = &swapchain_image_views[i];
        VkResult result = vkCreateFramebuffer(device, &fb_info, nullptr, &buffer);
        if(result != VK_SUCCESS){
            ENGINE_ERROR("failed to create framebuffer {0}", i);
        }
        main_framebuffer.push_back(buffer);
    }

}

void VulkanContext::DrawsObjects(){
    PrepareFrame();
    FrameData &current_frame = frames[frame_count % 2];

    VkClearValue color = {0.0, 0.0, 1.0, 1.0};
    VkRenderPassBeginInfo pass_info = {};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.pNext = nullptr;
    pass_info.renderPass = main_pass;
    pass_info.renderArea.offset = {0, 0};
    pass_info.clearValueCount = 1;
    pass_info.pClearValues = &color;
    pass_info.renderArea.extent = {swapchain_extent.width, swapchain_extent.height};
    pass_info.framebuffer = main_framebuffer[image_index];
    vkCmdBeginRenderPass(current_frame.main_buffer, &pass_info, VK_SUBPASS_CONTENTS_INLINE);
    //actually drawing
    vkCmdEndRenderPass(current_frame.main_buffer);
    SubmitFrame();
}

void VulkanContext::PrepareFrame(){
    FrameData &current_frame = frames[frame_count % 2];

    VkResult result = vkWaitForFences(device, 1, &current_frame.render_fence, true, 1000000000);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("Failed on fence");
    }
    result = vkResetFences(device, 1, &current_frame.render_fence);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("Failed to reset fence");
    }

    result = vkResetCommandBuffer(current_frame.main_buffer, 0);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("Failed to reset command buffer");
    }
    result = vkAcquireNextImageKHR(device,swapchain, 1000000000, current_frame.present_semaphore, nullptr,&image_index);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("Failed to acquire next image");
    }

    VkCommandBufferBeginInfo cmd_begin_info = {};
    cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    cmd_begin_info.pNext = nullptr;
    cmd_begin_info.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(current_frame.main_buffer, &cmd_begin_info);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("Failed to begin command buffer");
    }
}

void VulkanContext::SubmitFrame(){
    FrameData &current_frame = frames[frame_count % 2];
    VkResult result = vkEndCommandBuffer(current_frame.main_buffer);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("failed to end command buffer");
    }

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &current_frame.present_semaphore;
    submit.pWaitDstStageMask = &wait_stage;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &current_frame.main_buffer;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &current_frame.render_semaphore;

    result = vkQueueSubmit(graphics_queue, 1, &submit, current_frame.render_fence);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("failed to queue submit");
    }

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.pImageIndices = &image_index;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &current_frame.render_semaphore;

    result = vkQueuePresentKHR(graphics_queue, &present_info);
    if(result != VK_SUCCESS){
        ENGINE_ERROR("failed to queue present");
    }

    frame_count ++;
}

void VulkanContext::Cleanup(){
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}
