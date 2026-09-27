


#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>

typedef struct packet packet;

typedef struct ring_buffer{
    uint8_t *packets;
    uint8_t capacity;
    uint8_t tail;
    uint8_t head;
}ring_buffer;

void ring_buffer_init(uint8_t capacity);
uint8_t ring_buffer_write(uint8_t *buffer);
uint8_t ring_buffer_read(uint8_t *out_buffer);

uint8_t is_ring_buffer_full();
uint8_t is_ring_buffer_empty();
#endif //RING_BUFFER_H