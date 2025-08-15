internal void
renderer_init(platform_handle_t window_handle)
{
    arena_params_t params = { MB(32), MB(32), 0 };
    g_renderer.arena = arena_init(&params);

    renderer_vk_create_instance();
    renderer_vk_create_surface(window_handle);
    renderer_vk_create_physical_device();
    renderer_vk_create_queue_ids();
    renderer_vk_create_device();
    renderer_vk_create_swapchain(window_handle);
    renderer_vk_create_command_pool();
    renderer_vk_create_command_buffers();
    renderer_vk_create_descriptor_pool();
    renderer_vk_create_sync_primitives();

    vkGetPhysicalDeviceMemoryProperties(g_renderer.physical_device, &g_renderer.device_mem_props);

    vertex_t vertices[] = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.8f}, {0.0f, 1.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.8f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    };

    u32_t indices[] = {
        0, 3, 1,
        1, 3, 2
    };

    renderer_vk_create_mesh_data(vertices, 4, indices, 6);

    g_renderer.pipelines      = MEMORY_PUSH_ZERO(g_renderer.arena, renderer_pipeline_t, 1);
    g_renderer.pipeline_count = 1;
    
    renderer_pipeline_init(g_renderer.pipelines);
}

internal void
renderer_update(platform_handle_t window_handle)
{
    persist u32_t frame_id = 0;

    vkWaitForFences(g_renderer.device, 1, &g_renderer.fence_in_flight[frame_id], VK_TRUE, UINT64_MAX);

    u32_t img_id;
    VkResult vk_result = vkAcquireNextImageKHR(
        g_renderer.device,
        g_renderer.swapchain,
        UINT64_MAX,
        g_renderer.sem_img_avail[frame_id],
        VK_NULL_HANDLE,
        &img_id
    );

    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        renderer_vk_swapchain_recreate(window_handle);

        return;
    }
    else
    {
        EMBER_ASSERT(vk_result == VK_SUCCESS || vk_result == VK_SUBOPTIMAL_KHR);
    }

    vkResetFences(g_renderer.device, 1, &g_renderer.fence_in_flight[frame_id]);
    vkResetCommandBuffer(g_renderer.command_buffers[frame_id], 0);

    renderer_vk_command_buffer_record(&g_renderer.pipelines[0], frame_id, img_id);

    renderer_vk_ubo_t ubo;

    quat_t rotation = quat_from_axis_angle(&(vec3_t){0.0f, 1.0f, 0.0f}, (f32_t)platform_timer_since_start());

    ubo.model = mat4_model(
        &(vec3_t){0.0f, 0.0f, 0.0f},
        &rotation,
        &(vec3_t){1.0f, 1.0f, 1.0f}
    );

    ubo.view = mat4_look_at(
        &(vec3_t){0.0f, 2.0f, -2.0f},
        &(vec3_t){0.0f, 0.0f,  0.0f},
        &(vec3_t){0.0f, 1.0f,  0.0f}
    );

    platform_window_size_t client_size = platform_gfx_window_client_get_size(window_handle);
    f32_t aspect = (f32_t)client_size.width / (f32_t)client_size.height;

    ubo.proj = mat4_perspective(30.0f, aspect, 0.01f, 100.0f);

    memcpy(g_renderer.mesh_data.ubo_mapped[frame_id], &ubo, sizeof(ubo));

    VkSemaphoreSubmitInfo wait_sem_info = {0};
    wait_sem_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_sem_info.semaphore             = g_renderer.sem_img_avail[frame_id];
    wait_sem_info.value                 = 0;
    wait_sem_info.stageMask             = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    wait_sem_info.deviceIndex           = 0;

    VkSemaphoreSubmitInfo sgnl_sem_info = {0};
    sgnl_sem_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    sgnl_sem_info.semaphore             = g_renderer.sem_render_end[frame_id];
    sgnl_sem_info.value                 = 0;
    sgnl_sem_info.stageMask             = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    sgnl_sem_info.deviceIndex           = 0;

    VkCommandBufferSubmitInfo cmd_submit_info = {0};
    cmd_submit_info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_submit_info.commandBuffer             = g_renderer.command_buffers[frame_id];
    cmd_submit_info.deviceMask                = 0;

    VkSubmitInfo2 submit_info            = {0};
    submit_info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount   = 1;
    submit_info.pWaitSemaphoreInfos      = &wait_sem_info;
    submit_info.commandBufferInfoCount   = 1;
    submit_info.pCommandBufferInfos      = &cmd_submit_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos    = &sgnl_sem_info;

    vk_result = vkQueueSubmit2(g_renderer.graphics_queue, 1, &submit_info, g_renderer.fence_in_flight[frame_id]);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    VkPresentInfoKHR present_info   = {0};
    present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &g_renderer.sem_render_end[frame_id];
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &g_renderer.swapchain;
    present_info.pImageIndices      = &img_id;
    present_info.pResults           = NULL;

    vk_result = vkQueuePresentKHR(g_renderer.present_queue, &present_info);

    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR || g_window_state.is_resizing)
    {
        renderer_vk_swapchain_recreate(window_handle);
    }
    else
    {
        EMBER_ASSERT(vk_result == VK_SUCCESS);
    }

    frame_id = (frame_id + 1) % RENDERER_FRAMES_IN_FLIGHT;
}

internal void
renderer_destroy()
{
    vkDeviceWaitIdle(g_renderer.device);

    for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(g_renderer.device, g_renderer.sem_img_avail[i], NULL);
        vkDestroySemaphore(g_renderer.device, g_renderer.sem_render_end[i], NULL);
        vkDestroyFence(g_renderer.device, g_renderer.fence_in_flight[i], NULL);
    }

    for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        vkUnmapMemory(g_renderer.device, g_renderer.mesh_data.ubo_memory[i]);
        vkDestroyBuffer(g_renderer.device, g_renderer.mesh_data.ubo_buffer[i], NULL);
        vkFreeMemory(g_renderer.device, g_renderer.mesh_data.ubo_memory[i], NULL);
    }

    vkDestroyBuffer(g_renderer.device, g_renderer.mesh_data.vertex_buffer, NULL);
    vkDestroyBuffer(g_renderer.device, g_renderer.mesh_data.index_buffer, NULL);

    vkFreeMemory(g_renderer.device, g_renderer.mesh_data.vertex_memory, NULL);
    vkFreeMemory(g_renderer.device, g_renderer.mesh_data.index_memory, NULL);

    for (int i = 0; i < g_renderer.pipeline_count; i++)
    {
        renderer_pipeline_destroy(g_renderer.pipelines + i);
    }

    vkDestroyDescriptorPool(g_renderer.device, g_renderer.descriptor_pool, NULL);
    vkDestroyCommandPool(g_renderer.device, g_renderer.command_pool, NULL);

    for (u32_t i = 0; i < RENDERER_SWAP_IMG_COUNT; i++)
    {
        vkDestroyImageView(g_renderer.device, g_renderer.swapchain_img_views[i], NULL);
    }
    vkDestroySwapchainKHR(g_renderer.device, g_renderer.swapchain, NULL);
    vkDestroyDevice(g_renderer.device, NULL);
    vkDestroySurfaceKHR(g_renderer.instance, g_renderer.surface, NULL);
    vkDestroyInstance(g_renderer.instance, NULL);

    arena_release(g_renderer.arena);
}

internal void
renderer_pipeline_init(renderer_pipeline_t* pipeline)
{
    renderer_vk_pipeline_create_descriptor_set_layout(pipeline);
    renderer_vk_pipeline_create_descriptor_sets(pipeline);
    renderer_vk_pipeline_create_graphics_pipeline_layout(pipeline);
    renderer_vk_pipeline_create_graphics_pipeline(pipeline);
}

internal void
renderer_pipeline_destroy(renderer_pipeline_t* pipeline)
{
    vkDestroyPipeline(g_renderer.device, pipeline->graphics_pipeline, NULL);
    vkDestroyPipelineLayout(g_renderer.device, pipeline->graphics_pipeline_layout, NULL);

    // NOTE(KB): No need to free descriptor sets individually
    //           If we want to do this we need to create the pool with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
    //
    //for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    //{
    //    vkFreeDescriptorSets(g_renderer.device, g_renderer.descriptor_pool, RENDERER_FRAMES_IN_FLIGHT, pipeline->descriptor_sets);
    //}
    vkDestroyDescriptorSetLayout(g_renderer.device, pipeline->descriptor_set_layout, NULL);
}

internal void
renderer_vk_create_instance()
{
    u32_t vk_api_version = VK_API_VERSION_1_4;

    // TODO(KB): Provide appropriate names/versions for app/engine
    VkApplicationInfo app_info  = {0};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext              = NULL;
    app_info.pApplicationName   = NULL;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName        = "Ember Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion         = vk_api_version;

    PFN_vkEnumerateInstanceVersion vk_api_version_func = 
        (PFN_vkEnumerateInstanceVersion) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");

    // TODO(KB): We probably want to notify the user of unsupported/unavailable SDK version
    EMBER_ASSERT(vk_api_version_func);
    u32_t vk_api_version_supported = VK_API_VERSION_1_0;
    vk_api_version_func(&vk_api_version_supported);

    EMBER_ASSERT(vk_api_version_supported >= vk_api_version);

    b32_t exts_found = renderer_vk_check_instance_extensions();
    EMBER_ASSERT(exts_found);

#if RHI_VK_VALIDATIONS_ENABLED
    b32_t layers_found = renderer_vk_check_validation_layers();
#endif

    VkInstanceCreateInfo instance_info    = {0};
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

    VkResult create_result = vkCreateInstance(&instance_info, NULL, &g_renderer.instance);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void
renderer_vk_create_surface(platform_handle_t window_handle)
{
#if PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surface_info = {0};
    surface_info.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance                   = platform_get_instance_handle().hnd;
    surface_info.hwnd                        = window_handle.hnd;

    VkResult create_result = vkCreateWin32SurfaceKHR(g_renderer.instance, &surface_info, NULL, &g_renderer.surface);
    EMBER_ASSERT(create_result == VK_SUCCESS);
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

    EMBER_ASSERT(device_count > 0);

    VkPhysicalDevice* devices = MEMORY_PUSH(scratch.arena, VkPhysicalDevice, device_count);
    vkEnumeratePhysicalDevices(g_renderer.instance, &device_count, devices);

    for (u32_t i = 0; i < device_count; i++)
    {
        if (renderer_vk_device_is_suitable(devices[i]))
        {
            g_renderer.physical_device = devices[i];
            break;
        }
    }

    EMBER_ASSERT(g_renderer.physical_device != VK_NULL_HANDLE);

    arena_scratch_end(scratch);
}

internal void
renderer_vk_create_queue_ids()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(g_renderer.physical_device, &family_count, NULL);

    VkQueueFamilyProperties* family_props = MEMORY_PUSH(scratch.arena, VkQueueFamilyProperties, family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(g_renderer.physical_device, &family_count, family_props);

    b32_t graphics_found = EMBER_FALSE;
    b32_t present_found  = EMBER_FALSE;
    u32_t graphics_id    = 0;
    u32_t present_id     = 0;

    for (u32_t i = 0; i < family_count; i++)
    {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_renderer.physical_device, i, g_renderer.surface, &present_support);

        b32_t graphics_support = family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

        if (graphics_support && present_support)
        {
            graphics_found = EMBER_TRUE;
            graphics_id    = i;

            present_found = EMBER_TRUE;
            present_id    = i;

            break;
        }

        if (graphics_support && !graphics_found)
        {
            graphics_found = EMBER_TRUE;
            graphics_id    = i;
        }

        if (present_support && !present_found)
        {
            present_found = EMBER_TRUE;
            present_id    = i;
        }
    }

    EMBER_ASSERT(graphics_found && present_found);

    arena_scratch_end(scratch);

    g_renderer.queue_ids.graphics     = graphics_id;
    g_renderer.queue_ids.presentation = present_id;
}

internal void
renderer_vk_create_device()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t queue_count = (g_renderer.queue_ids.graphics == g_renderer.queue_ids.presentation) ? 1 : 2;

    VkDeviceQueueCreateInfo* queue_infos = MEMORY_PUSH_ZERO(scratch.arena, VkDeviceQueueCreateInfo, queue_count);

    f32_t queue_priority = 1.0f;
    for (u32_t i = 0; i < queue_count; i++)
    {
        queue_infos[i].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_infos[i].queueFamilyIndex = *(((u32_t *)(&g_renderer.queue_ids)) + i);
        queue_infos[i].queueCount       = 1;
        queue_infos[i].pQueuePriorities = &queue_priority;
    }

    // TODO(KB): Connect this to the feature check in ...device_is_suitable()
    VkPhysicalDeviceFeatures feats = {0};
    feats.samplerAnisotropy = VK_TRUE;

    VkPhysicalDeviceVulkan13Features feats_13 = {0};
    feats_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    feats_13.dynamicRendering = VK_TRUE;
    feats_13.synchronization2 = VK_TRUE;

    VkDeviceCreateInfo device_info      = {0};
    device_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext                   = &feats_13;
    device_info.queueCreateInfoCount    = queue_count;
    device_info.pQueueCreateInfos       = queue_infos;
    device_info.pEnabledFeatures        = &feats;
    device_info.enabledExtensionCount   = ARRAY_COUNT(g_device_extensions);
    device_info.ppEnabledExtensionNames = g_device_extensions;
#if RHI_VK_VALIDATIONS_ENABLED
    device_info.enabledLayerCount       = ARRAY_COUNT(g_validation_layers);
    device_info.ppEnabledLayerNames     = g_validation_layers;
#endif

    VkResult create_result = vkCreateDevice(g_renderer.physical_device, &device_info, NULL, &g_renderer.device);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    vkGetDeviceQueue(g_renderer.device, g_renderer.queue_ids.graphics, 0, &g_renderer.graphics_queue);
    vkGetDeviceQueue(g_renderer.device, g_renderer.queue_ids.presentation, 0, &g_renderer.present_queue);

    arena_scratch_end(scratch);
}

internal void
renderer_vk_create_swapchain(platform_handle_t window_handle)
{
    VkSurfaceFormatKHR swap_format = renderer_vk_swapchain_find_format();
    VkPresentModeKHR swap_present  = renderer_vk_swapchain_find_present();
    VkExtent2D swap_extent         = renderer_vk_swapchain_find_extent(window_handle);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_renderer.physical_device, g_renderer.surface, &capabilities);

    u32_t img_count = RENDERER_SWAP_IMG_COUNT;

    EMBER_ASSERT(img_count >= capabilities.minImageCount);
    EMBER_ASSERT(img_count <= capabilities.maxImageCount || capabilities.maxImageCount == 0);

    VkSwapchainCreateInfoKHR swap_info = {0};
    swap_info.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_info.surface                  = g_renderer.surface;
    swap_info.minImageCount            = img_count;
    swap_info.imageFormat              = swap_format.format;
    swap_info.imageColorSpace          = swap_format.colorSpace;
    swap_info.imageExtent              = swap_extent;
    swap_info.imageArrayLayers         = 1;
    swap_info.preTransform             = capabilities.currentTransform;
    swap_info.presentMode              = swap_present;
    swap_info.clipped                  = VK_TRUE;
    swap_info.oldSwapchain             = VK_NULL_HANDLE;
    swap_info.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (g_renderer.queue_ids.graphics != g_renderer.queue_ids.presentation)
    {
        swap_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swap_info.queueFamilyIndexCount = 2;
        swap_info.pQueueFamilyIndices   = (u32_t *)(&g_renderer.queue_ids);
    }
    else
    {
        swap_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swap_info.queueFamilyIndexCount = 0;
        swap_info.pQueueFamilyIndices   = NULL;
    }

    VkResult create_result = vkCreateSwapchainKHR(g_renderer.device, &swap_info, NULL, &g_renderer.swapchain);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    g_renderer.swapchain_extent    = swap_extent;
    g_renderer.swapchain_img_fmt   = swap_format.format;
    g_renderer.swapchain_images    = MEMORY_PUSH(g_renderer.arena, VkImage, img_count);
    g_renderer.swapchain_img_views = MEMORY_PUSH(g_renderer.arena, VkImageView, img_count);

    VkImageViewCreateInfo view_info           = {0};
    view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format                          = g_renderer.swapchain_img_fmt;
    view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.baseMipLevel   = 0;
    view_info.subresourceRange.levelCount     = 1;
    view_info.subresourceRange.layerCount     = 1;

    vkGetSwapchainImagesKHR(g_renderer.device, g_renderer.swapchain, &img_count, g_renderer.swapchain_images);
    for (u32_t i = 0; i < img_count; i++)
    {
        view_info.image = g_renderer.swapchain_images[i];

        create_result = vkCreateImageView(g_renderer.device, &view_info, NULL, &g_renderer.swapchain_img_views[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);
    }
}

internal void
renderer_vk_create_command_pool()
{
    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex        = g_renderer.queue_ids.graphics;

    VkResult create_result = vkCreateCommandPool(g_renderer.device, &pool_info, NULL, &g_renderer.command_pool);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void
renderer_vk_create_command_buffers()
{
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = g_renderer.command_pool;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = RENDERER_FRAMES_IN_FLIGHT;

    VkResult alloc_result = vkAllocateCommandBuffers(g_renderer.device, &alloc_info, (VkCommandBuffer *)&g_renderer.command_buffers);
    EMBER_ASSERT(alloc_result == VK_SUCCESS);
}

internal void
renderer_vk_create_descriptor_pool()
{
    VkDescriptorPoolSize pool_sizes[1] = {0};
    pool_sizes[0].type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount      = RENDERER_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount              = ARRAY_COUNT(pool_sizes);
    pool_info.pPoolSizes                 = pool_sizes;
    pool_info.maxSets                    = RENDERER_FRAMES_IN_FLIGHT;

    VkResult vk_result = vkCreateDescriptorPool(g_renderer.device, &pool_info, NULL, &g_renderer.descriptor_pool);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void
renderer_vk_create_sync_primitives()
{
    VkSemaphoreCreateInfo sem_info = {0};
    sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        VkResult create_result;

        create_result = vkCreateSemaphore(g_renderer.device, &sem_info, NULL, &g_renderer.sem_img_avail[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);

        create_result = vkCreateSemaphore(g_renderer.device, &sem_info, NULL, &g_renderer.sem_render_end[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);

        create_result = vkCreateFence(g_renderer.device, &fence_info, NULL, &g_renderer.fence_in_flight[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);
    }
}

internal void
renderer_vk_create_mesh_data(vertex_t* vertices, u32_t vertex_count, u32_t* indices, u32_t indices_count)
{
    VkDeviceSize vert_size = vertex_count * sizeof(vertex_t);
    VkDeviceSize idx_size  = indices_count * sizeof(indices[0]);
    VkDeviceSize max_size  = MAX(vert_size, idx_size);
    VkDeviceSize ubo_size  = sizeof(renderer_vk_ubo_t);

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    renderer_vk_create_buffer(
        &staging_buffer,
        max_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    );

    renderer_vk_create_buffer(
        &g_renderer.mesh_data.vertex_buffer,
        vert_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );

    renderer_vk_create_buffer(
        &g_renderer.mesh_data.index_buffer,
        idx_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    );

    for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        renderer_vk_create_buffer(
            &g_renderer.mesh_data.ubo_buffer[i],
            ubo_size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
        );
    }

    renderer_vk_create_buffer_memory(
        staging_buffer,
        &staging_memory,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    renderer_vk_create_buffer_memory(
        g_renderer.mesh_data.vertex_buffer,
        &g_renderer.mesh_data.vertex_memory,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    renderer_vk_create_buffer_memory(
        g_renderer.mesh_data.index_buffer,
        &g_renderer.mesh_data.index_memory,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        renderer_vk_create_buffer_memory(
            g_renderer.mesh_data.ubo_buffer[i],
            &g_renderer.mesh_data.ubo_memory[i],
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        );

        VkResult vk_result = vkMapMemory(g_renderer.device, g_renderer.mesh_data.ubo_memory[i], 0, ubo_size, 0, &g_renderer.mesh_data.ubo_mapped[i]);
        EMBER_ASSERT(vk_result == VK_SUCCESS);
    }

    void* data;
    VkResult vk_result = vkMapMemory(g_renderer.device, staging_memory, 0, max_size, 0, &data);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    memcpy(data, vertices, vert_size);

    renderer_vk_copy_buffer(staging_buffer, g_renderer.mesh_data.vertex_buffer, vert_size);

    memcpy(data, indices, idx_size);

    renderer_vk_copy_buffer(staging_buffer, g_renderer.mesh_data.index_buffer, idx_size);

    vkUnmapMemory(g_renderer.device, staging_memory);
    vkDestroyBuffer(g_renderer.device, staging_buffer, NULL);
    vkFreeMemory(g_renderer.device, staging_memory, NULL);

    g_renderer.mesh_data.indices_count = indices_count;
}

internal void
renderer_vk_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo buffer_info = {0};
    buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.flags              = 0;
    buffer_info.usage              = usage;
    buffer_info.size               = size;
    buffer_info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    VkResult vk_result = vkCreateBuffer(g_renderer.device, &buffer_info, NULL, buffer);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void
renderer_vk_create_buffer_memory(VkBuffer buffer, VkDeviceMemory* memory, VkMemoryPropertyFlags mem_flags)
{
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(g_renderer.device, buffer, &mem_req);

    u32_t memory_index = U32_MAX;
    for (u32_t i = 0; i < g_renderer.device_mem_props.memoryTypeCount; i++)
    {
        b32_t type_check = mem_req.memoryTypeBits & (1 << i);
        b32_t flag_check = (g_renderer.device_mem_props.memoryTypes[i].propertyFlags & mem_flags) == mem_flags;
        if (type_check && flag_check)
        {
            memory_index = i;
            break;
        }
    }

    EMBER_ASSERT(memory_index != U32_MAX);

    VkMemoryAllocateInfo alloc_info = {0};
    alloc_info.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize       = mem_req.size;
    alloc_info.memoryTypeIndex      = memory_index;

    VkResult vk_result = vkAllocateMemory(g_renderer.device, &alloc_info, NULL, memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vkBindBufferMemory(g_renderer.device, buffer, *memory, 0);
}

internal void
renderer_vk_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool                 = g_renderer.command_pool;
    alloc_info.commandBufferCount          = 1;

    VkCommandBuffer cmd_buffer;
    VkResult vk_result = vkAllocateCommandBuffers(g_renderer.device, &alloc_info, &cmd_buffer);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkBufferCopy copy_region = {0};
    copy_region.size         = size;

    vkCmdCopyBuffer(cmd_buffer, src, dst, 1, &copy_region);

    vkEndCommandBuffer(cmd_buffer);

    VkCommandBufferSubmitInfo cmd_buffer_info = {0};
    cmd_buffer_info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_buffer_info.commandBuffer             = cmd_buffer;

    VkSubmitInfo2 submit_info          = {0};
    submit_info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos    = &cmd_buffer_info;

    vkQueueSubmit2(g_renderer.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(g_renderer.graphics_queue);

    vkFreeCommandBuffers(g_renderer.device, g_renderer.command_pool, 1, &cmd_buffer);
}

internal void
renderer_vk_pipeline_create_descriptor_set_layout(renderer_pipeline_t* pipeline)
{
    VkDescriptorSetLayoutBinding bindings[1] = {0};
    bindings[0].binding                      = 0;
    bindings[0].descriptorCount              = 1;
    bindings[0].descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers           = NULL;

    VkDescriptorSetLayoutCreateInfo binding_info = {0};
    binding_info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    binding_info.bindingCount                    = ARRAY_COUNT(bindings);
    binding_info.pBindings                       = bindings;

    VkResult create_result = vkCreateDescriptorSetLayout(g_renderer.device, &binding_info, NULL, &pipeline->descriptor_set_layout);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void
renderer_vk_pipeline_create_descriptor_sets(renderer_pipeline_t* pipeline)
{
    VkDescriptorSetLayout layouts[RENDERER_FRAMES_IN_FLIGHT] = {
        pipeline->descriptor_set_layout,
        pipeline->descriptor_set_layout
    };

    VkDescriptorSetAllocateInfo alloc_info = {0};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool              = g_renderer.descriptor_pool;
    alloc_info.descriptorSetCount          = RENDERER_FRAMES_IN_FLIGHT;
    alloc_info.pSetLayouts                 = layouts;

    VkResult vk_result = vkAllocateDescriptorSets(g_renderer.device, &alloc_info, pipeline->descriptor_sets);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo ubo_info = {0};
        ubo_info.buffer                 = g_renderer.mesh_data.ubo_buffer[i];
        ubo_info.offset                 = 0;
        ubo_info.range                  = sizeof(renderer_vk_ubo_t);

        VkWriteDescriptorSet write_set[1] = {0};
        write_set[0].sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_set[0].dstSet               = pipeline->descriptor_sets[i];
        write_set[0].dstBinding           = 0;
        write_set[0].dstArrayElement      = 0;
        write_set[0].descriptorCount      = 1;
        write_set[0].descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_set[0].pImageInfo           = NULL;
        write_set[0].pBufferInfo          = &ubo_info;
        write_set[0].pTexelBufferView     = NULL;

        vkUpdateDescriptorSets(g_renderer.device, ARRAY_COUNT(write_set), write_set, 0, NULL);
    }
}

internal void
renderer_vk_pipeline_create_graphics_pipeline_layout(renderer_pipeline_t* pipeline)
{
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount             = 1;
    layout_info.pSetLayouts                = &pipeline->descriptor_set_layout;
    layout_info.pushConstantRangeCount     = 0;
    layout_info.pPushConstantRanges        = NULL;

    VkResult create_result = vkCreatePipelineLayout(g_renderer.device, &layout_info, NULL, &pipeline->graphics_pipeline_layout);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void
renderer_vk_pipeline_create_graphics_pipeline(renderer_pipeline_t* pipeline)
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);
    u8_t* vert        = MEMORY_PUSH(scratch.arena, u8_t, KB(16));
    u8_t* frag        = MEMORY_PUSH(scratch.arena, u8_t, KB(16));

    u64_t vert_size = platform_file_data("./triangle_vert.spv", vert);
    u64_t frag_size = platform_file_data("./triangle_frag.spv", frag);

    VkShaderModule vert_module = renderer_vk_pipeline_create_shader_module(vert, vert_size);
    VkShaderModule frag_module = renderer_vk_pipeline_create_shader_module(frag, frag_size);

    VkPipelineShaderStageCreateInfo shader_info[2] = {0};
    shader_info[0].sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_info[0].stage                           = VK_SHADER_STAGE_VERTEX_BIT;
    shader_info[0].pName                           = "main";
    shader_info[0].module                          = vert_module;
    shader_info[0].pSpecializationInfo             = NULL;

    shader_info[1].sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_info[1].stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_info[1].pName                           = "main";
    shader_info[1].module                          = frag_module;
    shader_info[1].pSpecializationInfo             = NULL;

    VkVertexInputBindingDescription vertex_bindings = {0};
    vertex_bindings.binding                         = 0;
    vertex_bindings.stride                          = sizeof(vertex_t);
    vertex_bindings.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_attr[VERTEX_ATTR_TYPE_count] = {0};
    vertex_attr[VERTEX_ATTR_TYPE_position].binding                        = 0;
    vertex_attr[VERTEX_ATTR_TYPE_position].location                       = 0;
    vertex_attr[VERTEX_ATTR_TYPE_position].offset                         = offsetof(vertex_t, position);
    vertex_attr[VERTEX_ATTR_TYPE_position].format                         = VK_FORMAT_R32G32B32_SFLOAT;

    vertex_attr[VERTEX_ATTR_TYPE_normal].binding                          = 0;
    vertex_attr[VERTEX_ATTR_TYPE_normal].location                         = 1;
    vertex_attr[VERTEX_ATTR_TYPE_normal].offset                           = offsetof(vertex_t, normal);
    vertex_attr[VERTEX_ATTR_TYPE_normal].format                           = VK_FORMAT_R32G32B32_SFLOAT;

    vertex_attr[VERTEX_ATTR_TYPE_color].binding                           = 0;
    vertex_attr[VERTEX_ATTR_TYPE_color].location                          = 2;
    vertex_attr[VERTEX_ATTR_TYPE_color].offset                            = offsetof(vertex_t, color);
    vertex_attr[VERTEX_ATTR_TYPE_color].format                            = VK_FORMAT_R32G32B32_SFLOAT;

    vertex_attr[VERTEX_ATTR_TYPE_uv].binding                              = 0;
    vertex_attr[VERTEX_ATTR_TYPE_uv].location                             = 3;
    vertex_attr[VERTEX_ATTR_TYPE_uv].offset                               = offsetof(vertex_t, uv);
    vertex_attr[VERTEX_ATTR_TYPE_uv].format                               = VK_FORMAT_R32G32_SFLOAT;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
    vertex_input_info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount        = 1;
    vertex_input_info.pVertexBindingDescriptions           = &vertex_bindings;
    vertex_input_info.vertexAttributeDescriptionCount      = VERTEX_ATTR_TYPE_count;
    vertex_input_info.pVertexAttributeDescriptions         = vertex_attr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {0};
    input_assembly_info.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable                 = VK_FALSE;

    // NOTE(KB): We will use dynamic state so we dont need to specify a VkViewport and VkRect2D (viewport and scissor)

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamic_info = {0};
    dynamic_info.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_info.dynamicStateCount                = ARRAY_COUNT(dynamic_states);
    dynamic_info.pDynamicStates                   = dynamic_states;

    VkPipelineViewportStateCreateInfo viewport_info = {0};
    viewport_info.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount                     = 1;
    viewport_info.scissorCount                      = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer_info = {0};
    rasterizer_info.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_info.depthClampEnable                       = VK_FALSE;
    rasterizer_info.rasterizerDiscardEnable                = VK_FALSE;
    rasterizer_info.polygonMode                            = VK_POLYGON_MODE_FILL;
    rasterizer_info.lineWidth                              = 1.0f;
    rasterizer_info.cullMode                               = VK_CULL_MODE_BACK_BIT;
    rasterizer_info.frontFace                              = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer_info.depthBiasEnable                        = VK_FALSE;
    rasterizer_info.depthBiasConstantFactor                = 0.0f;
    rasterizer_info.depthBiasClamp                         = 0.0f;
    rasterizer_info.depthBiasSlopeFactor                   = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample_info = {0};
    multisample_info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.sampleShadingEnable                  = VK_FALSE;
    multisample_info.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;
    multisample_info.minSampleShading                     = 1.0f;
    multisample_info.pSampleMask                          = NULL;
    multisample_info.alphaToCoverageEnable                = VK_FALSE;
    multisample_info.alphaToOneEnable                     = VK_FALSE;

    VkStencilOpState stencil_state_front = {0};
    VkStencilOpState stencil_state_back  = {0};

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {0};
    depth_stencil_info.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_info.depthTestEnable                       = VK_TRUE;
    depth_stencil_info.depthWriteEnable                      = VK_TRUE;
    depth_stencil_info.depthCompareOp                        = VK_COMPARE_OP_LESS;
    depth_stencil_info.depthBoundsTestEnable                 = VK_FALSE;
    depth_stencil_info.minDepthBounds                        = 0.0f;
    depth_stencil_info.maxDepthBounds                        = 1.0f;
    depth_stencil_info.stencilTestEnable                     = VK_FALSE;
    depth_stencil_info.front                                 = stencil_state_front;
    depth_stencil_info.back                                  = stencil_state_back;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.blendEnable                         = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor                 = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor                 = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp                        = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp                        = VK_BLEND_OP_ADD;
    color_blend_attachment.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT |
                                                                 VK_COLOR_COMPONENT_G_BIT |
                                                                 VK_COLOR_COMPONENT_B_BIT |
                                                                 VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo color_blend_info = {0};
    color_blend_info.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.logicOpEnable                       = VK_FALSE;
    color_blend_info.logicOp                             = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount                     = 1;
    color_blend_info.pAttachments                        = &color_blend_attachment;
    color_blend_info.blendConstants[0]                   = 0.0f;
    color_blend_info.blendConstants[1]                   = 0.0f;
    color_blend_info.blendConstants[2]                   = 0.0f;
    color_blend_info.blendConstants[3]                   = 0.0f;

    VkPipelineRenderingCreateInfo rendering_info = {0};
    rendering_info.sType                         = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.viewMask                      = 0;
    rendering_info.colorAttachmentCount          = 1;
    rendering_info.pColorAttachmentFormats       = &g_renderer.swapchain_img_fmt;
    rendering_info.depthAttachmentFormat         = VK_FORMAT_D32_SFLOAT; // TODO(KB): g_renderer.depth_img_fmt
    rendering_info.stencilAttachmentFormat       = VK_FORMAT_UNDEFINED;

    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.pNext                        = &rendering_info;
    graphics_pipeline_info.stageCount                   = 2;
    graphics_pipeline_info.pStages                      = shader_info;
    graphics_pipeline_info.pVertexInputState            = &vertex_input_info;
    graphics_pipeline_info.pInputAssemblyState          = &input_assembly_info;
    graphics_pipeline_info.pViewportState               = &viewport_info;
    graphics_pipeline_info.pRasterizationState          = &rasterizer_info;
    graphics_pipeline_info.pMultisampleState            = &multisample_info;
    graphics_pipeline_info.pDepthStencilState           = &depth_stencil_info;
    graphics_pipeline_info.pColorBlendState             = &color_blend_info;
    graphics_pipeline_info.pDynamicState                = &dynamic_info;
    graphics_pipeline_info.layout                       = pipeline->graphics_pipeline_layout;
    graphics_pipeline_info.renderPass                   = NULL;
    graphics_pipeline_info.subpass                      = 0;
    graphics_pipeline_info.basePipelineHandle           = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex            = -1;

    VkResult create_result = vkCreateGraphicsPipelines(g_renderer.device, VK_NULL_HANDLE, 1, &graphics_pipeline_info, NULL, &pipeline->graphics_pipeline);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    vkDestroyShaderModule(g_renderer.device, vert_module, NULL);
    vkDestroyShaderModule(g_renderer.device, frag_module, NULL);

    arena_scratch_end(scratch);
}

internal VkShaderModule
renderer_vk_pipeline_create_shader_module(const u8_t* code, u64_t code_size)
{
    VkShaderModuleCreateInfo module_info = {0};
    module_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_info.codeSize                 = code_size;
    module_info.pCode                    = (u32_t *)code;

    VkShaderModule result;

    VkResult create_result = vkCreateShaderModule(g_renderer.device, &module_info, NULL, &result);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    return result;
}

internal void
renderer_vk_swapchain_recreate(platform_handle_t window_handle)
{
    while(platform_gfx_window_is_minimized(window_handle))
    {
        platform_gfx_process_events();
    }

    vkDeviceWaitIdle(g_renderer.device);

    for (u32_t i = 0; i < RENDERER_SWAP_IMG_COUNT; i++)
    {
        vkDestroyImageView(g_renderer.device, g_renderer.swapchain_img_views[i], NULL);
    }
    vkDestroySwapchainKHR(g_renderer.device, g_renderer.swapchain, NULL);

    renderer_vk_create_swapchain(window_handle);
}

internal VkSurfaceFormatKHR
renderer_vk_swapchain_find_format()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_renderer.physical_device, g_renderer.surface, &format_count, NULL);

    EMBER_ASSERT(format_count > 0);

    VkSurfaceFormatKHR* formats = MEMORY_PUSH(scratch.arena, VkSurfaceFormatKHR, format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_renderer.physical_device, g_renderer.surface, &format_count, formats);

    VkSurfaceFormatKHR result = {
        VK_FORMAT_B8G8R8_SRGB,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    };

    if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        arena_scratch_end(scratch);

        return result;
    }

    for (u32_t i = 0; i < format_count; i++)
    {
        if (formats[i].format == result.format && formats[i].colorSpace == result.colorSpace)
        {
            arena_scratch_end(scratch);

            return result;
        }
    }

    result = formats[0];

    arena_scratch_end(scratch);

    return result;
}

internal VkPresentModeKHR
renderer_vk_swapchain_find_present()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t present_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_renderer.physical_device, g_renderer.surface, &present_count, NULL);

    VkPresentModeKHR* present_modes = MEMORY_PUSH(scratch.arena, VkPresentModeKHR, present_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_renderer.physical_device, g_renderer.surface, &present_count, present_modes);

    for (u32_t i = 0; i < present_count; i++)
    {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            arena_scratch_end(scratch);
            return present_modes[i];
        }
    }

    // NOTE(KB): VK_PRESENT_MODE_FIFO_KHR is the only one guaranteed to exist
    arena_scratch_end(scratch);
    return VK_PRESENT_MODE_FIFO_KHR;
}

internal VkExtent2D
renderer_vk_swapchain_find_extent(platform_handle_t window_handle)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_renderer.physical_device, g_renderer.surface, &capabilities);

    if (capabilities.currentExtent.width != U32_MAX)
    {
        return capabilities.currentExtent;
    }

    platform_window_size_t client_size = platform_gfx_window_client_get_size(window_handle);

    VkExtent2D extent;

    extent.width  = CLAMP(client_size.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = CLAMP(client_size.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}

internal void
renderer_vk_command_buffer_record(renderer_pipeline_t* pipeline, u32_t buffer_id, u32_t img_id)
{
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags                    = 0;
    begin_info.pInheritanceInfo         = NULL;

    VkCommandBuffer cmd = g_renderer.command_buffers[buffer_id];

    VkResult cmd_result = vkBeginCommandBuffer(cmd, &begin_info);
    EMBER_ASSERT(cmd_result == VK_SUCCESS);

    VkClearValue clear_value_color = {{ 0.03f, 0.07f, 0.10f, 1.0f }};
    VkClearValue clear_value_depth = { 1.0f, 0.0f };

    VkRenderingAttachmentInfo color_attachment = {0};
    color_attachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.pNext                     = NULL;
    color_attachment.imageView                 = g_renderer.swapchain_img_views[img_id];
    color_attachment.imageLayout               = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    color_attachment.resolveMode               = VK_RESOLVE_MODE_NONE;
    color_attachment.resolveImageView          = VK_NULL_HANDLE;
    color_attachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.clearValue                = clear_value_color;

    VkRenderingAttachmentInfo depth_attachment = {0};
    depth_attachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.pNext                     = NULL;
    depth_attachment.imageView                 = NULL;//g_renderer.swapchain_img_views[img_id];
    depth_attachment.imageLayout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment.resolveMode               = VK_RESOLVE_MODE_NONE;
    depth_attachment.resolveImageView          = VK_NULL_HANDLE;
    depth_attachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.clearValue                = clear_value_depth;

    VkRenderingInfo rendering_info      = {0};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.pNext                = NULL;
    rendering_info.flags                = 0;
    rendering_info.renderArea.offset.x  = 0;
    rendering_info.renderArea.offset.y  = 0;
    rendering_info.renderArea.extent    = g_renderer.swapchain_extent;
    rendering_info.layerCount           = 1;
    rendering_info.viewMask             = 0;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments    = &color_attachment;
    rendering_info.pDepthAttachment     = NULL;
    rendering_info.pStencilAttachment   = NULL;

    VkViewport viewport = {0};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = (f32_t)g_renderer.swapchain_extent.width;
    viewport.height     = (f32_t)g_renderer.swapchain_extent.height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent   = g_renderer.swapchain_extent;

    VkImageMemoryBarrier2 img_barrier           = {0};
    img_barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    img_barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    img_barrier.srcAccessMask                   = 0;
    img_barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    img_barrier.dstAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    img_barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    img_barrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    img_barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier.image                           = g_renderer.swapchain_images[img_id];
    img_barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    img_barrier.subresourceRange.baseMipLevel   = 0;
    img_barrier.subresourceRange.levelCount     = 1;
    img_barrier.subresourceRange.baseArrayLayer = 0;
    img_barrier.subresourceRange.layerCount     = 1;

    VkDependencyInfo dep_info        = {0};
    dep_info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep_info.dependencyFlags         = 0;
    dep_info.imageMemoryBarrierCount = 1;
    dep_info.pImageMemoryBarriers    = &img_barrier;

    vkCmdPipelineBarrier2(cmd, &dep_info);

    vkCmdBeginRendering(cmd, &rendering_info);

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphics_pipeline);

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(cmd, 0, 1, &g_renderer.mesh_data.vertex_buffer, offsets);
    vkCmdBindIndexBuffer(cmd, g_renderer.mesh_data.index_buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->graphics_pipeline_layout,
        0,
        1,
        &pipeline->descriptor_sets[buffer_id],
        0,
        NULL
    );

    vkCmdDrawIndexed(cmd, g_renderer.mesh_data.indices_count, 1, 0, 0, 0);

    vkCmdEndRendering(cmd);

    VkImageMemoryBarrier2 img_barrier_end           = {0};
    img_barrier_end.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    img_barrier_end.srcStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    img_barrier_end.srcAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    img_barrier_end.dstStageMask                    = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    img_barrier_end.dstAccessMask                   = 0;
    img_barrier_end.oldLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    img_barrier_end.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    img_barrier_end.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier_end.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier_end.image                           = g_renderer.swapchain_images[img_id];
    img_barrier_end.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    img_barrier_end.subresourceRange.baseMipLevel   = 0;
    img_barrier_end.subresourceRange.levelCount     = 1;
    img_barrier_end.subresourceRange.baseArrayLayer = 0;
    img_barrier_end.subresourceRange.layerCount     = 1;

    VkDependencyInfo dep_info_end        = {0};
    dep_info_end.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep_info_end.dependencyFlags         = 0;
    dep_info_end.imageMemoryBarrierCount = 1;
    dep_info_end.pImageMemoryBarriers    = &img_barrier_end;

    vkCmdPipelineBarrier2(cmd, &dep_info_end);

    cmd_result = vkEndCommandBuffer(cmd);
    EMBER_ASSERT(cmd_result == VK_SUCCESS);
}

internal b32_t
renderer_vk_check_validation_layers()
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties* layer_props = MEMORY_PUSH(scratch.arena, VkLayerProperties, layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layer_props);

    b32_t layers_found = EMBER_TRUE;
    for (u32_t i = 0; i < ARRAY_COUNT(g_validation_layers); i++)
    {
        b32_t layer_found = EMBER_FALSE;
        for (u32_t j = 0; j < layer_count; j++)
        {
            if (strcmp(g_validation_layers[i], layer_props[j].layerName) == 0)
            {
                layer_found = EMBER_TRUE;
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

    b32_t exts_found = EMBER_TRUE;
    for (u32_t i = 0; i < ARRAY_COUNT(g_instance_extensions); i++)
    {
        b32_t ext_found = EMBER_FALSE;
        for (u32_t j = 0; j < ext_count; j++)
        {
            if (strcmp(g_instance_extensions[i], ext_props[j].extensionName) == 0)
            {
                ext_found = EMBER_TRUE;
                break;
            }
        }

        exts_found &= ext_found;
    }

    arena_scratch_end(scratch);

    return exts_found;
}

internal b32_t
renderer_vk_check_device_extensions(VkPhysicalDevice device)
{
    scratch_t scratch = arena_scratch_begin(g_renderer.arena);

    u32_t ext_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);

    VkExtensionProperties* ext_props = MEMORY_PUSH(scratch.arena, VkExtensionProperties, ext_count);
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, ext_props);

    b32_t exts_found = EMBER_TRUE;
    for (u32_t i = 0; i < ARRAY_COUNT(g_device_extensions); i++)
    {
        b32_t ext_found = EMBER_FALSE;
        for (u32_t j = 0; j < ext_count; j++)
        {
            if (strcmp(g_device_extensions[i], ext_props[j].extensionName) == 0)
            {
                ext_found = EMBER_TRUE;
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
    VkPhysicalDeviceProperties2 props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceVulkan13Features feats_13 = {0};
    feats_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 feats ={0};
    feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    feats.pNext = &feats_13;

    vkGetPhysicalDeviceProperties2(device, &props);
    vkGetPhysicalDeviceFeatures2(device, &feats);

    b32_t exts_supported = renderer_vk_check_device_extensions(device);
    b32_t prop_supported = (props.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    b32_t feat_supported = feats.features.samplerAnisotropy && feats_13.dynamicRendering && feats_13.synchronization2;

    b32_t result = 
        exts_supported &&
        prop_supported &&
        feat_supported;

    return result;
}

