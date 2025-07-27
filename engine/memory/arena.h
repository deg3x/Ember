#ifndef ARENA_H
#define ARENA_H

#define ARENA_HEADER_SIZE 64 

// Hard typed enum size
typedef u64_t arena_flags_t;
enum
{
    ARENA_FLAGS_large_pages = (1 << 0),
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
    u64_t size_res;
    u64_t size_cmt;
    u64_t position;
    arena_flags_t flags;
};

internal arena_t* arena_init(arena_params_t* params);
internal void*    arena_push(arena_t* arena, u64_t size, u64_t align);
internal void     arena_pop(arena_t* arena, u64_t size);
internal void     arena_pop_to(arena_t* arena, u64_t pos);
internal void     arena_clear(arena_t* arena);
internal void     arena_release(arena_t* arena);

#endif // ARENA_H
