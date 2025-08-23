#ifndef PLATFORM_H
#define PLATFORM_H

typedef struct platform_info_t platform_info_t;
struct platform_info_t
{
    u64_t page_size;
    u64_t page_size_large;
    u64_t alloc_granularity;
    u64_t processor_count;
};

typedef struct platform_handle_t platform_handle_t;
struct platform_handle_t
{
    void* hnd;
};

typedef struct platform_window_size_t platform_window_size_t;
struct platform_window_size_t
{
    u32_t width;
    u32_t height;
};

typedef struct platform_window_state_t platform_window_state_t;
struct platform_window_state_t
{
    b32_t is_resizing;
};

typedef struct platform_program_state_t platform_program_state_t;
struct platform_program_state_t
{
    b32_t is_running;
};

typedef struct platform_timer_t platform_timer_t;
struct platform_timer_t
{
    u64_t frequency;
    f64_t inv_freq;
    u64_t start;
    u64_t last;
    u64_t now;
};

typedef u32_t platform_file_flags_t;
enum
{
    PLATFORM_FILE_FLAGS_read    = (1 << 0),
    PLATFORM_FILE_FLAGS_write   = (1 << 1),
    PLATFORM_FILE_FLAGS_exec    = (1 << 2),
    PLATFORM_FILE_FLAGS_append  = (1 << 3),
    PLATFORM_FILE_FLAGS_share_r = (1 << 4),
    PLATFORM_FILE_FLAGS_share_w = (1 << 5),
};

#define FILE_READ_ALL U64_MAX

typedef struct platform_file_props_t platform_file_props_t;
struct platform_file_props_t
{
    u64_t size;
};

global platform_info_t          g_platform_info;
global platform_timer_t         g_timer;
global platform_program_state_t g_program_state;
global platform_window_state_t  g_window_state;

internal void platform_info_init();
internal void platform_abort(i32_t exit_code);

internal void  platform_timer_init();
internal void  platform_timer_update();
internal f64_t platform_timer_since_start();
internal f64_t platform_timer_delta();

internal void* platform_mem_reserve(u64_t size);
internal void* platform_mem_reserve_large(u64_t size);
internal b32_t platform_mem_commit(void* ptr, u64_t size);
internal b32_t platform_mem_commit_large(void* ptr, u64_t size);
internal void  platform_mem_release(void* ptr, u64_t size);
internal void  platform_mem_decommit(void* ptr, u64_t size);

internal platform_handle_t     platform_file_open(const char* file_path, platform_file_flags_t flags);
internal void                  platform_file_close(platform_handle_t file_handle);
internal platform_file_props_t platform_file_props(platform_handle_t file_handle);
internal u64_t                 platform_file_write(platform_handle_t file_handle, void* data, u64_t write_size);
internal u64_t                 platform_file_read(platform_handle_t file_handle, void* data, u64_t read_size);
internal u64_t                 platform_file_data(const char* file_path, void* data);

internal b32_t             platform_handle_equal(platform_handle_t handle_a, platform_handle_t handle_b);
internal platform_handle_t platform_get_instance_handle();

internal void                   platform_gfx_init();
internal void                   platform_gfx_process_events();
internal platform_handle_t      platform_gfx_window_create(const char* title);
internal platform_window_size_t platform_gfx_window_get_size(platform_handle_t window_handle);
internal platform_window_size_t platform_gfx_window_client_get_size(platform_handle_t window_handle);
internal b32_t                  platform_gfx_window_is_minimized(platform_handle_t window_handle);

#endif // PLATFORM_H
