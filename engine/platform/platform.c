internal u64_t
platform_file_data(const char* file_path, void* data)
{
    platform_file_flags_t read_flags = PLATFORM_FILE_FLAGS_read;
    platform_handle_t file_handle    = platform_file_open(file_path, read_flags);
    platform_file_props_t props      = platform_file_props(file_handle);

    u64_t result = platform_file_read(file_handle, data, props.size);
    platform_file_close(file_handle);

    return result;
}

internal b32_t
platform_handle_equal(platform_handle_t handle_a, platform_handle_t handle_b)
{
    return handle_a.hnd == handle_b.hnd;
}
