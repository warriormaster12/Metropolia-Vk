cmake_minimum_required(VERSION 3.0.0)
project(Metropolia-Vk VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)


file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*)
add_executable(Metropolia-Vk ${SRC})

target_include_directories(Metropolia-Vk PUBLIC
    libs/glm 
    libs/spdlog
    libs/vk-bootstrap
    libs/vma
    
    src/logger
    src/window
)

if (UNIX AND NOT APPLE)
    set (LINUX TRUE)
	target_compile_definitions(Metropolia-Vk PRIVATE -DVK_USE_PLATFORM_XLIB_KHR)
	target_include_directories(Metropolia-Vk PRIVATE Vulkan::Vulkan)
	target_include_directories(Metropolia-Vk PRIVATE ${X11+INCLUDE_DIR})
	target_link_libraries(Metropolia-Vk PRIVATE Vulkan::Vulkan)
	target_link_libraries(Metropolia-Vk PRIVATE ${X11_LIBRARIES} -lm)
endif ()

if (WIN32)
	target_compile_definitions(Metropolia-Vk PRIVATE -DVK_USE_PLATFORM_WIN32LIB_KHR)
	target_include_directories(Metropolia-Vk PRIVATE Vulkan::Vulkan)
	target_include_directories(Metropolia-Vk PRIVATE ${WIN32+INCLUDE_DIR})
	target_link_libraries(Metropolia-Vk PRIVATE Vulkan::Vulkan)
	target_link_libraries(Metropolia-Vk PRIVATE ${WIN32_LIBRARIES})   
endif ()

#glfw
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
target_link_libraries(Metropolia-Vk PRIVATE glfw)
