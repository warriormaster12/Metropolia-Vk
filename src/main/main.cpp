#include <iostream>
#include "window.h"
#include "logger.h"
#include "vk_context.h"

VulkanContext context;

int main(int argc, char* argv[]) {
    Logger::Init();
    MetroWindow::Init(1280, 720);
    context.Init(MetroWindow::GetWindowPointer());
    context.CreateSwapchain(MetroWindow::GetWidth(), MetroWindow::GetHeight());
    context.InitCommandBuffers();
    context.InitRenderpass();
    while(MetroWindow::Running()) {
        context.DrawsObjects();
    }
    context.Cleanup();
    MetroWindow::Shutdown();
    return 0;
}
