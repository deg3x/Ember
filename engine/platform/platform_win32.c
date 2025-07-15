typedef struct win32_window_t
{
    HWND handle;
    HDC device_ctx;
} win32_window_t;

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

void win32_window_init(HINSTANCE instance)
{
    WNDCLASSEX window_class = {};

    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc   = win32_window_message_callback;
    window_class.cbClsExtra    = 0;
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = instance;
    window_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName  = NULL;
    window_class.lpszClassName = "ember-window";
    window_class.hIconSm       = NULL;

    RegisterClassEx(&window_class);
}

win32_window_t win32_window_create(HINSTANCE instance, const char* window_name)
{
    win32_window_t window = {};

    HWND window_handle = CreateWindowEx(
        0,
        "ember-window",
        window_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        instance,
        NULL
    );

    if (window_handle == NULL)
    {
        return window;
    }

    DragAcceptFiles(window_handle, 1);

    window.handle     = window_handle;
    window.device_ctx = GetDC(window_handle);

    return window;
}

bool_t win32_window_process_events()
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

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    OutputDebugStringA("Engine is running...\n");

    win32_window_init(instance);
    win32_window_t window = win32_window_create(instance, "Ember Engine");

    ShowWindow(window.handle, SW_SHOW);

    while (win32_window_process_events())
    {

    }

    return 0;
}

