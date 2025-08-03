#ifndef RENDERER_VULKAN_H 
#define RENDERER_VULKAN_H 

struct renderer_t
{
    arena_t* arena;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;
};

typedef struct renderer_vk_queue_indices_t renderer_vk_queue_indices_t;
struct renderer_vk_queue_indices_t
{
    u32_t graphics;
    u32_t presentation;
};

global renderer_t g_renderer;

global const char* g_validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

global const char* g_instance_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if PLATFORM_WINDOWS
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
};

global const char* g_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

internal void renderer_vk_create_instance();
internal void renderer_vk_create_surface(platform_handle_t window_handle);
internal void renderer_vk_create_physical_device();
internal void renderer_vk_create_device();
internal void renderer_vk_create_swapchain();

internal renderer_vk_queue_indices_t renderer_vk_find_queue_indices(VkPhysicalDevice device);

internal b32_t renderer_vk_check_validation_layers();
internal b32_t renderer_vk_check_instance_extensions();
internal b32_t renderer_vk_check_swapchain(VkPhysicalDevice device);
internal b32_t renderer_vk_check_device_extensions(VkPhysicalDevice device);
internal b32_t renderer_vk_device_is_suitable(VkPhysicalDevice device);

#endif // RENDERER_VULKAN_H 
