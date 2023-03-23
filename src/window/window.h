#pragma once

#include <iostream>
#include <string>

class MetroWindow{
public:
    static void Init(const int& w, const int& h, const std::string& name = "Hello window");
    static bool Running();
    static void Shutdown();
    static void ResizeFramebuffer(const bool& value) {framebuffer_resized = value;}
    static void* GetWindowPointer() {return p_window;}
    static uint32_t GetWidth() {return width;}
    static uint32_t GetHeight() {return height;}
private:
    static inline void* p_window = nullptr;
    static inline bool framebuffer_resized = false;
    static inline uint32_t width;
    static inline uint32_t height;
};
