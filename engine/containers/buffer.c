internal b32_t
buffer_is_valid(buffer_t* buffer)
{
    b32_t result = (buffer->data != NULL);

    return result;
}

internal b32_t
buffer_is_valid_idx(buffer_t* buffer, u64_t idx)
{
    b32_t result = (buffer->size > idx);

    return result;
}

internal b32_t
buffer_is_equal(buffer_t* lhs, buffer_t* rhs)
{
    if (lhs->size != rhs->size)
    {
        return EMBER_FALSE;
    }

    for (u64_t i = 0; i < lhs->size; i++)
    {
        if (lhs->data[i] != rhs->data[i])
        {
            return EMBER_FALSE;
        }
    }

    return EMBER_TRUE;
}
