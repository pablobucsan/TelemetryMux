

#include "../../include/common/packet.h"
#include "../../include/common/ring_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static ring_buffer rbuffer;


void ring_buffer_init(uint8_t capacity)
{
    rbuffer.capacity = capacity;
    rbuffer.head = 0;
    rbuffer.tail = 0;
    rbuffer.packets = malloc(sizeof(packet) * capacity);

    if (rbuffer.packets == NULL){
        printf("Failed to init ring buffer\n");
        exit(1);
    }

}



uint8_t ring_buffer_write(uint8_t *buffer)
{
    if (is_ring_buffer_full()){
        return 0;
    }

    void *slot = &rbuffer.packets[rbuffer.head & (rbuffer.capacity - 1)];
    memcpy(slot, buffer, sizeof(packet));
    rbuffer.head++;

    return 1;
}


uint8_t ring_buffer_read(uint8_t *out_buffer)
{
    if (is_ring_buffer_empty()){
        return 0;
    }

    void *slot = &rbuffer.packets[rbuffer.tail & (rbuffer.capacity - 1)];
    memcpy(out_buffer, slot, sizeof(packet));
    rbuffer.tail--;

    return 1;
}

uint8_t is_ring_buffer_empty()
{
    return rbuffer.tail == rbuffer.head;
}

uint8_t is_ring_buffer_full()
{
    return (rbuffer.head - rbuffer.tail == rbuffer.capacity);
}
