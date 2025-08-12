#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

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

global win32_gfx_state_t g_win32_gfx_state;

internal LRESULT CALLBACK win32_window_message_callback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

#endif // PLATFORM_WIN32_H
