#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t capacity;
    size_t size;
    size_t read;
    char *data;
} buffer_t;

// returns 1 on success, 0 on failure

// capacity is set to the smallest power of two which is greater then 'size'
int buffer_init(buffer_t *buf, size_t size)
{
    size_t len = 8;
    while (len < size)
        len *= 2;
    size = len;

    buf->data = (char * )malloc(size);
    if (!buf->data)
        return 0;
    buf->capacity = size;
    buf->size = 0;
    buf->read = 0;
    return 1;
}

buffer_t *create_buffer(size_t size)
{
    buffer_t *buf = (buffer_t *)malloc(sizeof(buffer_t));
    if (!buf)
        return NULL;
    if (!buffer_init(buf, size))
        free(buf);
    return buf;
}

int assure_capacity_buffer(buffer_t *buf, size_t needed_capacity)
{
    size_t new_capacity = buf->capacity;
    while (new_capacity < needed_capacity)
        new_capacity *= 2;

    void *new_data = realloc(buf->data, new_capacity);

    if (!new_data)
        return 0;

    buf->capacity = new_capacity;
    buf->data = new_data;
    return 1;
}

size_t buffer_write(buffer_t *buf, char *data_to_append, size_t data_len)
{
    if (!assure_capacity_buffer(buf, buf->size + data_len))
        return 0;

    memcpy(buf->data + buf->size, data_to_append, data_len);
    buf->size += data_len;
    return data_len;
}

size_t buffer_read(buffer_t *buf, char *dest, size_t max_len)
{
    if (max_len > buf->size - buf->read)
        max_len = buf->size - buf->read;
    memcpy(dest, buf->data + buf->read, max_len);
    buf->read += max_len;
    return max_len;
}

#endif