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
    u64_t u64;
};

platform_info_t g_platform_info = {};

void platform_info_init();
void platform_abort(i32_t exit_code);

void* platform_mem_reserve(u64_t size);
void* platform_mem_reserve_large(u64_t size);
b32_t platform_mem_commit(void* ptr, u64_t size);
b32_t platform_mem_commit_large(void* ptr, u64_t size);
void  platform_mem_release(void* ptr, u64_t size);
void  platform_mem_decommit(void* ptr, u64_t size);

void              platform_gfx_init();
b32_t             platform_gfx_process_events();
platform_handle_t platform_gfx_window_create(const char* title);
