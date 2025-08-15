internal arena_t*
arena_init(arena_params_t* params)
{
    EMBER_ASSERT(ARENA_HEADER_SIZE >= sizeof(arena_t));
    EMBER_ASSERT(params->size_reserve >= params->size_commit);
    EMBER_ASSERT(params->size_commit >= ARENA_HEADER_SIZE);

    b64_t lrg_pages = params->flags & ARENA_FLAGS_large_pages; 
    u64_t page_size = lrg_pages ? g_platform_info.page_size_large : g_platform_info.page_size;

    u64_t reserve = POW_2_ROUND_UP(params->size_reserve, page_size);
    u64_t commit  = POW_2_ROUND_UP(params->size_commit, page_size);

    void* address = NULL;
    
    if (lrg_pages)
    {
        address = platform_mem_reserve_large(reserve);
        platform_mem_commit_large(address, commit);
    }
    else
    {
        address = platform_mem_reserve(reserve);
        platform_mem_commit(address, commit);
    }

    EMBER_ASSERT(address != 0);

    if (address == 0)
    {
        platform_abort(1);
    }

    arena_t* result  = (arena_t*)address;
    result->size_res = reserve;
    result->size_cmt = commit;
    result->position = ARENA_HEADER_SIZE;
    result->flags    = params->flags;

    return result;
}

internal void*
arena_push(arena_t* arena, u64_t size, u64_t align)
{
    u64_t pos_start = POW_2_ROUND_UP(arena->position, align);
    u64_t pos_goal  = POW_2_ROUND_UP(pos_start + size, align);

    EMBER_ASSERT(arena->size_res >= pos_goal);

    if (arena->size_cmt < pos_goal)
    {
        u64_t commit = CLAMP_TOP(pos_goal, arena->size_res);
        commit      -= arena->size_cmt;

        if (arena->flags & ARENA_FLAGS_large_pages)
        {
            commit = POW_2_ROUND_UP(commit, g_platform_info.page_size_large);
            platform_mem_commit_large(arena, commit);
        }
        else
        {
            commit = POW_2_ROUND_UP(commit, g_platform_info.page_size);
            platform_mem_commit(arena, commit);
        }

        pos_goal        = commit;
        arena->size_cmt = commit;
    }

    u8_t* result    = (u8_t*)arena + arena->position;
    arena->position = pos_goal;

    return result;
}

internal void
arena_pop(arena_t* arena, u64_t size)
{
    EMBER_ASSERT(arena->position >= size);

    arena->position -= size;
}

internal void
arena_pop_to(arena_t* arena, u64_t pos)
{
    EMBER_ASSERT(arena->position >= pos);

    arena->position = pos;
}

internal void
arena_clear(arena_t* arena)
{
    arena_pop_to(arena, 0);
}

internal void
arena_release(arena_t* arena)
{
    platform_mem_release(arena, arena->size_res);
}

internal scratch_t
arena_scratch_begin(arena_t* arena)
{
    scratch_t scratch = {
        arena,
        arena->position
    };

    return scratch;
}

internal void
arena_scratch_end(scratch_t scratch)
{
    arena_pop_to(scratch.arena, scratch.position);
}
