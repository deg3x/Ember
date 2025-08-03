internal void
renderer_init(platform_handle_t window_handle)
{
    arena_params_t params = { KB(64), KB(64), 0 };
    g_renderer.arena = arena_init(&params);

    renderer_vk_create_instance();
    renderer_vk_create_surface(window_handle);
    renderer_vk_create_physical_device();
    renderer_vk_create_device();
}

internal void
renderer_shutdown()
{
    vkDestroyDevice(g_renderer.device, NULL);
    vkDestroySurfaceKHR(g_renderer.instance, g_renderer.surface, NULL);
    vkDestroyInstance(g_renderer.instance, NULL);

    arena_release(g_renderer.arena);
}

internal void
renderer_vk_create_instance()
{
    // TODO(KB): Provide appropriate names/versions for app/engine
    VkApplicationInfo app_info  = {};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext              = NULL;
    app_info.pApplicationName   = NULL;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName        = "Ember Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion         = VK_API_VERSION_1_0;

    b32_t exts_found = renderer_vk_check_instance_extensions();
    EMBER_ASSERT(exts_found);

#if RHI_VK_VALIDATIONS_ENABLED
    b8_t layers_found = renderer_vk_check_validation_layers();
#endif

    VkInstanceCreateInfo instance_info    = {};
    instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo        = &app_info;
    instance_info.enabledExtensionCount   = ARRAY_COUNT(g_instance_extensions);
    instance_info.ppEnabledExtensionNames = g_instance_extensions;
    
#if RHI_VK_VALIDATIONS_ENABLED
    if (layers_found)
    {
        instance_info.enabledLayerCount   = ARRAY_COUNT(g_validation_layers);
        instance_info.ppEnabledLayerNames = g_validation_layers;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
    }
#endif

    VkResult result = vkCreateInstance(&instance_info, NULL, &g_renderer.instance);
    EMBER_ASSERT(result == VK_SUCCESS);
}

internal void
renderer_vk_create_surface(platform_handle_t window_handle)
{
#if PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surface_info = {};
    surface_info.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance                   = platform_get_instance_handle().hnd;
    surface_info.hwnd                        = window_handle.hnd;

    VkResult result = vkCreateWin32SurfaceKHR(g_renderer.instance, &surface_info, NULL, &g_renderer.surface);
    EMBER_ASSERT(result == VK_SUCCESS);
#else
    #error "Platform renderer surface not supported"
#endif
}

internal void
renderer_vk_create_physical_device()
{
    g_renderer.physical_device = VK_NULL_HANDLE;

    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t device_count = 0;
    vkEnumeratePhysicalDevices(g_renderer.instance, &device_count, NULL);

    VkPhysicalDevice* devices = MEMORY_PUSH(scratch.arena, VkPhysicalDevice, device_count);
    vkEnumeratePhysicalDevices(g_renderer.instance, &device_count, devices);

    for (int i = 0; i < ARRAY_COUNT(devices); i++)
    {
        if (renderer_vk_device_is_suitable(devices[i]))
        {
            g_renderer.physical_device = devices[i];
            break;
        }
    }

    arena_scratch_end(scratch);

    EMBER_ASSERT(g_renderer.physical_device != VK_NULL_HANDLE);
}

internal void
renderer_vk_create_device()
{
    renderer_vk_queue_indices_t family_indices = renderer_vk_find_queue_indices(g_renderer.physical_device);

    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t queue_count = (family_indices.graphics == family_indices.presentation) ? 1 : 2;

    VkDeviceQueueCreateInfo* queue_infos = MEMORY_PUSH_ZERO(scratch.arena, VkDeviceQueueCreateInfo, queue_count);

    f32_t queue_priority = 1.0f;
    for (u32_t i = 0; i < queue_count; i++)
    {
        queue_infos[i].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_infos[i].queueFamilyIndex = *(((u32_t *)(&family_indices)) + i);
        queue_infos[i].queueCount       = 1;
        queue_infos[i].pQueuePriorities = &queue_priority;
    }

    // TODO(KB): Connect this to the feature check in ...device_is_suitable()
    VkPhysicalDeviceFeatures features = {};
    features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_info      = {};
    device_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount    = queue_count;
    device_info.pQueueCreateInfos       = queue_infos;
    device_info.pEnabledFeatures        = &features;
    device_info.enabledExtensionCount   = ARRAY_COUNT(g_device_extensions);
    device_info.ppEnabledExtensionNames = g_device_extensions;
#if RHI_VK_VALIDATIONS_ENABLED
    device_info.enabledLayerCount       = ARRAY_COUNT(g_validation_layers);
    device_info.ppEnabledLayerNames     = g_validation_layes;
#endif

    VkResult result = vkCreateDevice(g_renderer.physical_device, &device_info, NULL, &g_renderer.device);

    arena_scratch_end(scratch);

    EMBER_ASSERT(result == VK_SUCCESS);
}

internal void
renderer_vk_create_swapchain()
{

}

internal renderer_vk_queue_indices_t
renderer_vk_find_queue_indices(VkPhysicalDevice device)
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, NULL);

    VkQueueFamilyProperties* family_props = MEMORY_PUSH(scratch.arena, VkQueueFamilyProperties, family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, family_props);

    b8_t graphics_found = FALSE;
    b8_t present_found  = FALSE;
    u32_t graphics      = 0;
    u32_t present       = 0;

    for (u32_t i = 0; (i < family_count) && !(graphics_found && present_found); i++)
    {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, g_renderer.surface, &present_support);

        if (family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphics_found = TRUE;
            graphics       = i;
        }

        if (present_support)
        {
            present_found = TRUE;
            present       = i;
        }
    }

    arena_scratch_end(scratch);

    EMBER_ASSERT(graphics_found && present_found);

    renderer_vk_queue_indices_t result = {
        graphics,
        present
    };

    return result;
}

internal b32_t
renderer_vk_check_validation_layers()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties* layer_props = MEMORY_PUSH(scratch.arena, VkLayerProperties, layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layer_props);

    b32_t layers_found = TRUE;
    for (u32_t i = 0; i < ARRAY_COUNT(g_validation_layers); i++)
    {
        b32_t layer_found = FALSE;
        for (u32_t j = 0; j < layer_count; j++)
        {
            if (strcmp(g_validation_layers[i], layer_props[j].layerName) == 0)
            {
                layer_found = TRUE;
                break;
            }
        }

        layers_found &= layer_found;
    }

    arena_scratch_end(scratch);

    return layers_found;
}

internal b32_t
renderer_vk_check_instance_extensions()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t ext_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &ext_count, NULL);

    VkExtensionProperties* ext_props = MEMORY_PUSH(scratch.arena, VkExtensionProperties, ext_count);
    vkEnumerateInstanceExtensionProperties(NULL, &ext_count, ext_props);

    b32_t exts_found = TRUE;
    for (u32_t i = 0; i < ARRAY_COUNT(g_instance_extensions); i++)
    {
        b32_t ext_found = FALSE;
        for (u32_t j = 0; j < ext_count; j++)
        {
            if (strcmp(g_instance_extensions[i], ext_props[j].extensionName) == 0)
            {
                ext_found = TRUE;
                break;
            }
        }

        exts_found &= ext_found;
    }

    arena_scratch_end(scratch);

    return exts_found;
}

internal b32_t
renderer_vk_check_swapchain(VkPhysicalDevice device)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, g_renderer.surface, &capabilities);

    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_renderer.surface, &format_count, NULL);

    VkSurfaceFormatKHR* formats = MEMORY_PUSH(scratch.arena, VkSurfaceFormatKHR, format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_renderer.surface, &format_count, formats);

    u32_t present_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_renderer.surface, &present_count, NULL);

    VkPresentModeKHR* present_modes = MEMORY_PUSH(scratch.arena, VkPresentModeKHR, present_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_renderer.surface, &present_count, present_modes);

    arena_scratch_end(scratch);

    return TRUE;
}

internal b32_t
renderer_vk_check_device_extensions(VkPhysicalDevice device)
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t ext_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);

    VkExtensionProperties* ext_props = MEMORY_PUSH(scratch.arena, VkExtensionProperties, ext_count);
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, ext_props);

    b32_t exts_found = TRUE;
    for (u32_t i = 0; i < ARRAY_COUNT(g_device_extensions); i++)
    {
        b32_t ext_found = FALSE;
        for (u32_t j = 0; j < ext_count; j++)
        {
            if (strcmp(g_device_extensions[i], ext_props[j].extensionName) == 0)
            {
                ext_found = TRUE;
                break;
            }
        }

        exts_found &= ext_found;
    }

    arena_scratch_end(scratch);

    return exts_found;
}

internal b32_t
renderer_vk_device_is_suitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    b32_t exts_supported = renderer_vk_check_device_extensions(device);
    b32_t swap_supported = renderer_vk_check_swapchain(device);
    b32_t prop_supported = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    b32_t feat_supported = features.samplerAnisotropy;

    b32_t result = 
        exts_supported &&
        swap_supported &&
        prop_supported &&
        feat_supported;

    return result;
}

