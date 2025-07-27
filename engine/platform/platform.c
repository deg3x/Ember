internal b32_t
platform_handle_equal(platform_handle_t handle_a, platform_handle_t handle_b)
{
    return handle_a.u64 == handle_b.u64;
}
