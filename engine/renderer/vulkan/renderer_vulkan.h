#ifndef RENDERER_VULKAN_H 
#define RENDERER_VULKAN_H 

typedef struct renderer_vk_queue_ids_t renderer_vk_queue_ids_t;
struct renderer_vk_queue_ids_t
{
    u32_t graphics;
    u32_t presentation;
};

typedef struct renderer_vk_ubo_t renderer_vk_ubo_t;
struct renderer_vk_ubo_t
{
    mat4_t model;
    mat4_t view;
    mat4_t proj;
};

typedef struct renderer_mesh_data_t renderer_mesh_data_t;
struct renderer_mesh_data_t
{
    VkBuffer       vertex_buffer;
    VkBuffer       index_buffer;
    VkBuffer       ubo_buffer[RENDERER_FRAMES_IN_FLIGHT];
    VkDeviceMemory vertex_memory;
    VkDeviceMemory index_memory;
    VkDeviceMemory ubo_memory[RENDERER_FRAMES_IN_FLIGHT];

    void*          ubo_mapped[RENDERER_FRAMES_IN_FLIGHT];
    u32_t          indices_count;
};

struct renderer_t
{
    arena_t*                         arena;
    VkInstance                       instance;
    VkSurfaceKHR                     surface;
    VkPhysicalDevice                 physical_device;
    VkPhysicalDeviceMemoryProperties device_mem_props;
    VkDevice                         device;
    renderer_vk_queue_ids_t          queue_ids;
    VkQueue                          graphics_queue;
    VkQueue                          present_queue;
    VkSwapchainKHR                   swapchain;
    VkImage*                         swapchain_images;
    VkImageView*                     swapchain_img_views;
    VkExtent2D                       swapchain_extent;
    VkFormat                         swapchain_img_fmt;
    VkCommandPool                    command_pool;
    VkDescriptorPool                 descriptor_pool;
    VkCommandBuffer                  command_buffers[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore                      sem_img_avail[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore                      sem_render_end[RENDERER_FRAMES_IN_FLIGHT];
    VkFence                          fence_in_flight[RENDERER_FRAMES_IN_FLIGHT];

    renderer_mesh_data_t             mesh_data;

    renderer_pipeline_t*             pipelines;
    u64_t                            pipeline_count;
};

struct renderer_pipeline_t
{
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet       descriptor_sets[RENDERER_FRAMES_IN_FLIGHT];
    VkPipelineLayout      graphics_pipeline_layout;
    VkPipeline            graphics_pipeline;
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
internal void renderer_vk_create_queue_ids();
internal void renderer_vk_create_device();
internal void renderer_vk_create_swapchain(platform_handle_t window_handle);
internal void renderer_vk_create_command_pool();
internal void renderer_vk_create_command_buffers();
internal void renderer_vk_create_descriptor_pool();
internal void renderer_vk_create_sync_primitives();

internal void renderer_vk_create_mesh_data(vertex_t* vertices, u32_t vertex_count, u32_t* indices, u32_t indices_count);
internal void renderer_vk_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage);
internal void renderer_vk_create_buffer_memory(VkBuffer buffer, VkDeviceMemory* memory, VkMemoryPropertyFlags mem_flags);
internal void renderer_vk_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

internal void           renderer_vk_pipeline_create_descriptor_set_layout(renderer_pipeline_t* pipeline);
internal void           renderer_vk_pipeline_create_descriptor_sets(renderer_pipeline_t* pipeline);
internal void           renderer_vk_pipeline_create_graphics_pipeline_layout(renderer_pipeline_t* pipeline);
internal void           renderer_vk_pipeline_create_graphics_pipeline(renderer_pipeline_t* pipeline);
internal VkShaderModule renderer_vk_pipeline_create_shader_module(const u8_t* code, u64_t code_size);

internal void               renderer_vk_swapchain_recreate(platform_handle_t window_handle);
internal VkSurfaceFormatKHR renderer_vk_swapchain_find_format();
internal VkPresentModeKHR   renderer_vk_swapchain_find_present();
internal VkExtent2D         renderer_vk_swapchain_find_extent(platform_handle_t window_handle);

internal void renderer_vk_command_buffer_record(renderer_pipeline_t* pipeline, u32_t buffer_id, u32_t img_id);

internal b32_t renderer_vk_check_validation_layers();
internal b32_t renderer_vk_check_instance_extensions();
internal b32_t renderer_vk_check_device_extensions(VkPhysicalDevice device);
internal b32_t renderer_vk_device_is_suitable(VkPhysicalDevice device);

#endif // RENDERER_VULKAN_H 
