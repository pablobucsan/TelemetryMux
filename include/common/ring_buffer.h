


#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

typedef struct packet packet;

typedef struct ring_buffer{
    uint8_t *packets;
    uint8_t capacity;
    uint8_t tail;
    uint8_t head;
    uint8_t count;
    pthread_mutex_t lock;
}ring_buffer;

typedef enum ring_status{
    RING_SUCCESS,
    RING_ERR_BUSY,
    RING_ERR_FULL,
    RING_ERR_EMPTY
}ring_status;


void ring_buffer_init(uint8_t capacity);
uint8_t ring_buffer_write(uint8_t *buffer);
uint8_t ring_buffer_read(uint8_t *out_buffer);
uint8_t ring_buffer_count();
#endif //RING_BUFFER_H