#pragma once

#include <iostream>
#include <string>

class MetroWindow{
public:
    static void Init(const int& width, const int& height, const std::string& name = "Hello window");
    static bool Running();
    static void Shutdown();
    static void ResizeFramebuffer(const bool& value) {framebuffer_resized = value;}
private:
    static inline void* p_window = nullptr;
    static inline bool framebuffer_resized = false;
};
