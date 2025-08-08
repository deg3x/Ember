internal void
platform_info_init()
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    g_platform_info.page_size_large   = GetLargePageMinimum();
    g_platform_info.page_size         = system_info.dwPageSize;
    g_platform_info.processor_count   = system_info.dwNumberOfProcessors;
    g_platform_info.alloc_granularity = system_info.dwAllocationGranularity;
}

internal void
platform_abort(i32_t exit_code)
{
    ExitProcess(exit_code);
}

internal void*
platform_mem_reserve(u64_t size)
{
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);

    return result;
}

internal void*
platform_mem_reserve_large(u64_t size)
{
    // NOTE(KB): Windows requires large pages to be committed on reserve
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);

    return result;
}

internal b32_t
platform_mem_commit(void* ptr, u64_t size)
{
    b32_t result = (b32_t)(VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);

    return result;
}

internal b32_t
platform_mem_commit_large(void* ptr, u64_t size)
{
    return EMBER_TRUE;
}

internal void
platform_mem_release(void* ptr, u64_t size)
{
    // NOTE(KB): Windows requires size to be 0 for release
    VirtualFree(ptr, 0, MEM_RELEASE);
}

internal void
platform_mem_decommit(void* ptr, u64_t size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal platform_handle_t
platform_file_open(const char* file_path, platform_file_flags_t flags)
{
    DWORD access_flags = 0;
    DWORD share_flags  = 0;
    DWORD create_disp  = 0;

    if (flags & PLATFORM_FILE_FLAGS_read)
    {
        access_flags |= GENERIC_READ;
        create_disp   = OPEN_EXISTING;
    }
    if (flags & PLATFORM_FILE_FLAGS_exec)
    {
        access_flags |= GENERIC_EXECUTE;
        create_disp   = OPEN_EXISTING;
    }
    if (flags & PLATFORM_FILE_FLAGS_write)
    {
        access_flags |= GENERIC_WRITE;
        create_disp   = CREATE_ALWAYS;
    }
    if (flags & PLATFORM_FILE_FLAGS_append)
    {
        access_flags |= FILE_GENERIC_WRITE ^ FILE_WRITE_DATA;
        create_disp   = OPEN_ALWAYS;
    }
    if (flags & PLATFORM_FILE_FLAGS_share_r)
    {
        share_flags |= FILE_SHARE_READ;
    }
    if (flags & PLATFORM_FILE_FLAGS_share_w)
    {
        share_flags |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    }

    HANDLE file_handle = CreateFile(
        file_path,
        access_flags,
        share_flags,
        NULL,
        create_disp,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    platform_handle_t result = {};

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        return result;
    }

    result.hnd = file_handle;

    return result;
}

internal void
platform_file_close(platform_handle_t file_handle)
{
    if (file_handle.hnd == NULL)
    {
        return;
    }

    BOOL result = CloseHandle(file_handle.hnd);

    (void)result;
}

internal platform_file_props_t
platform_file_props(platform_handle_t file_handle)
{
    platform_file_props_t result = {};

    if (file_handle.hnd == NULL)
    {
        return result;
    }

    BY_HANDLE_FILE_INFORMATION file_info;
    BOOL info_valid = GetFileInformationByHandle(file_handle.hnd, &file_info);

    if (!info_valid)
    {
        return result;
    }

    result.size |= ((u64_t)file_info.nFileSizeHigh) << 32;
    result.size |= (u64_t)file_info.nFileSizeLow;

    return result;
}

// TODO(KB): Add writing to specified position (OVERLAPPED type)
internal u64_t
platform_file_write(platform_handle_t file_handle, void* data, u64_t write_size)
{
    if (file_handle.hnd == NULL)
    {
        return 0;
    }

    u64_t bytes_written_total = 0;

    while (bytes_written_total < write_size)
    {
        u64_t bytes_to_write_ttl = write_size - bytes_written_total;
        DWORD bytes_to_write     = U32_FROM_U64_CLAMPED(bytes_to_write_ttl);
        DWORD bytes_written;

        BOOL write_success = WriteFile(file_handle.hnd, data, bytes_to_write, &bytes_written, NULL);
        if (!write_success)
        {
            break;
        }

        bytes_written_total += bytes_written;
    }

    return bytes_written_total;
}

// TODO(KB): Add reading from specified position (OVERLAPPED type)
internal u64_t
platform_file_read(platform_handle_t file_handle, void* data, u64_t read_size)
{
    if (file_handle.hnd == NULL)
    {
        return 0;
    }

    u64_t file_size;
    BOOL size_success = GetFileSizeEx(file_handle.hnd, (LPDWORD)&file_size);
    if (!size_success)
    {
        return 0;
    }

    u64_t bytes_read_total  = 0;
    u64_t read_size_clamped = MIN(read_size, file_size);

    while (bytes_read_total < read_size_clamped)
    {
        u64_t bytes_to_read_ttl = read_size_clamped - bytes_read_total;
        DWORD bytes_to_read     = U32_FROM_U64_CLAMPED(bytes_to_read_ttl);
        DWORD bytes_read;

        BOOL read_success = ReadFile(file_handle.hnd, data, bytes_to_read, &bytes_read, NULL);
        if (!read_success)
        {
            break;
        }

        bytes_read_total += bytes_read;

        if (bytes_read != bytes_to_read)
        {
            break;
        }
    }

    return bytes_read_total;
}


internal platform_handle_t
platform_get_instance_handle()
{
    platform_handle_t handle = {
        .hnd = GetModuleHandle(NULL)
    };

    return handle;
}

internal void
platform_gfx_init()
{
    g_win32_gfx_state.instance = GetModuleHandle(NULL);

    WNDCLASSEX window_class    = {};
    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc   = win32_window_message_callback;
    window_class.cbClsExtra    = 0;
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = g_win32_gfx_state.instance;
    window_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName  = NULL;
    window_class.lpszClassName = "ember-window-class";
    window_class.hIconSm       = NULL;

    ATOM atom = RegisterClassEx(&window_class);
    (void)atom;
}

internal b32_t
platform_gfx_process_events()
{
    MSG msg = {};

    if (GetMessage(&msg, NULL, 0, 0) == 0)
    {
        return EMBER_FALSE;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);

    return EMBER_TRUE;
}

internal platform_handle_t
platform_gfx_window_create(const char* window_name)
{
    win32_window_t window = {};

    HWND window_handle = CreateWindowEx(
        0,
        "ember-window-class",
        window_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        g_win32_gfx_state.instance,
        NULL
    );

    EMBER_ASSERT(window_handle != NULL);

    DragAcceptFiles(window_handle, 1);

    window.handle     = window_handle;
    window.device_ctx = GetDC(window_handle);

    platform_handle_t handle = { window_handle};

    return handle;
}

internal platform_window_size_t
platform_gfx_window_get_size(platform_handle_t window_handle)
{
    RECT window_rect;
    
    EMBER_ASSERT(IsWindow(window_handle.hnd));

    GetWindowRect(window_handle.hnd, &window_rect);

    platform_window_size_t result = {
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top
    };

    return result;
}

internal platform_window_size_t
platform_gfx_window_client_get_size(platform_handle_t window_handle)
{
    RECT client_rect;
    
    EMBER_ASSERT(IsWindow(window_handle.hnd));

    GetClientRect(window_handle.hnd, &client_rect);

    platform_window_size_t result = {
        client_rect.right - client_rect.left,
        client_rect.bottom - client_rect.top
    };

    return result;
}

internal LRESULT CALLBACK
win32_window_message_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_SIZE:
        {
            return 0;
        }
        case WM_ACTIVATEAPP:
        {
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT:
        {
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

int WINAPI 
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    platform_gfx_init();
    platform_handle_t window_handle = platform_gfx_window_create("Ember Engine");

    platform_info_init();
    renderer_init(window_handle);

    ShowWindow((HWND)window_handle.hnd, SW_SHOW);

    while (platform_gfx_process_events())
    {

    }

    renderer_shutdown();

    return 0;
}

