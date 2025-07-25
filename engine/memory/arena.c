#define ARENA_HEADER_SIZE 128 

// Hard typed enum size
typedef u64_t arena_flags_t;
enum
{
    ARENA_FLAGS_no_chain    = (1 << 0),
    ARENA_FLAGS_large_pages = (1 << 1),
};

typedef struct arena_params_t arena_params_t;
struct arena_params_t
{
    u64_t size_reserve;
    u64_t size_commit;
    arena_flags_t flags;
};

typedef struct arena_t arena_t;
struct arena_t
{
    arena_t* prev;
    arena_t* curr;
#if ARENA_FREE_LIST
    arena_t* free_last;
    u64_t free_size;
#endif
    u64_t size_res_req;
    u64_t size_cmt_req;
    u64_t size_res;
    u64_t size_cmt;
    u64_t pos_base;
    u64_t pos_curr;
    arena_flags_t flags;
};

internal arena_t* arena_init(arena_params_t* params);
internal u64_t    arena_position(arena_t* arena);
internal void     arena_push(arena_t* arena, u64_t size, u64_t align);
internal void     arena_pop(arena_t* arena, u64_t size);
internal void     arena_pop_to(arena_t* arena, u64_t pos);
internal void     arena_clear(arena_t* arena);
internal void     arena_release(arena_t* arena);

////////////////
// Definitions

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

    EMBER_ASSERT(address != 0)

    if (address == 0)
    {
        platform_abort(1);
    }

    arena_t* result      = (arena_t*)address;
    result->curr         = result;
    result->prev         = NULL;
    result->size_res_req = params->size_reserve;
    result->size_cmt_req = params->size_commit;
    result->size_res     = reserve;
    result->size_cmt     = commit;
    result->pos_base     = 0;
    result->pos_curr     = ARENA_HEADER_SIZE;
    result->flags        = params->flags;
#if ARENA_FREE_LIST
    result->free_last    = NULL;
    result->free_size    = 0;
#endif

    return result;
}
