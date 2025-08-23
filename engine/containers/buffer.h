#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t buffer_t;
struct buffer_t
{
    u64_t size;
    u8_t* data;
};

internal b32_t buffer_is_valid(buffer_t* buffer);
internal b32_t buffer_is_valid_idx(buffer_t* buffer, u64_t idx);
internal b32_t buffer_is_equal(buffer_t* lhs, buffer_t* rhs);

#endif //BUFFER_H
