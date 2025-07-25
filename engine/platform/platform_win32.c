typedef struct win32_window_t win32_window_t;
struct win32_window_t
{
    HWND handle;
    HDC device_ctx;
};

typedef struct win32_gfx_state_t win32_gfx_state_t;
struct win32_gfx_state_t
{
    HINSTANCE instance;
};

win32_gfx_state_t g_win32_gfx_state = {};

LRESULT CALLBACK win32_window_message_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

void platform_info_init()
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    g_platform_info.page_size_large   = GetLargePageMinimum();
    g_platform_info.page_size         = system_info.dwPageSize;
    g_platform_info.processor_count   = system_info.dwNumberOfProcessors;
    g_platform_info.alloc_granularity = system_info.dwAllocationGranularity;
}

void platform_abort(i32_t exit_code)
{
    ExitProcess(exit_code);
}

void* platform_mem_reserve(u64_t size)
{
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);

    return result;
}

void* platform_mem_reserve_large(u64_t size)
{
    // Windows requires large pages to be committed on reserve
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);

    return result;
}

b32_t platform_mem_commit(void* ptr, u64_t size)
{
    b32_t result = (b32_t)(VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);

    return result;
}

b32_t platform_mem_commit_large(void* ptr, u64_t size)
{
    return TRUE;
}

void platform_mem_release(void* ptr, u64_t size)
{
    // Windows requires size to be 0 for release
    VirtualFree(ptr, 0, MEM_RELEASE);
}

void platform_mem_decommit(void* ptr, u64_t size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void platform_gfx_init()
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

platform_handle_t platform_gfx_window_create(const char* window_name)
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

    platform_handle_t handle = {(u64_t) window_handle};

    return handle;
}

b32_t platform_gfx_process_events()
{
    MSG msg = {};

    if (GetMessage(&msg, NULL, 0, 0) == 0)
    {
        return FALSE;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);

    return TRUE;
}

LRESULT CALLBACK win32_window_message_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
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

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    platform_gfx_init();
    platform_handle_t window_handle = platform_gfx_window_create("Ember Engine");

    ShowWindow((HWND)window_handle.u64, SW_SHOW);

    while (platform_gfx_process_events())
    {

    }

    return 0;
}

