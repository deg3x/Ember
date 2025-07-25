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
    u64_t size_cmt_req;
    u64_t size_res_req;
    u64_t size_cmt;
    u64_t size_res;
    u64_t pos_base;
    u64_t pos_curr;
    arena_flags_t flags;
};
