

#include "../../include/common/packet.h"
#include "../../include/common/ring_buffer.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

static ring_buffer rbuffer;


void ring_buffer_init(uint8_t capacity)
{
    rbuffer.capacity = capacity;
    rbuffer.head = 0;
    rbuffer.tail = 0;
    rbuffer.count = 0;
    pthread_mutex_init(&rbuffer.lock, NULL);
    rbuffer.packets = malloc(PACKET_SIZE * capacity);



    if (rbuffer.packets == NULL){
        printf("Failed to init ring buffer\n");
        exit(1);
    }

}


/**
 * trylock() does not block the execution of the calling thread
 * if the mutex is already locked by another thread, as lock() would do
 */
uint8_t ring_buffer_write(uint8_t *buffer)
{
    /** Try to acquire the lock */
    int error_code = pthread_mutex_trylock(&rbuffer.lock);

    /** If failed to  acquire it, return*/
    if (error_code != 0){
        return RING_ERR_BUSY;
    }

    /** We have the lock now */

    /** Check if there is room to write */
    if (rbuffer.count == rbuffer.capacity){
        pthread_mutex_unlock(&rbuffer.lock);
        return RING_ERR_FULL;
    }

    void *slot = &rbuffer.packets[rbuffer.head * PACKET_SIZE];
    memcpy(slot, buffer, PACKET_SIZE);
    rbuffer.head = (rbuffer.head + 1) % rbuffer.capacity;
    rbuffer.count++;
    pthread_mutex_unlock(&rbuffer.lock);

    return RING_SUCCESS;
}


uint8_t ring_buffer_read(uint8_t *out_buffer)
{
    int error_code = pthread_mutex_trylock(&rbuffer.lock);

    if (error_code != 0){
        return RING_ERR_BUSY;
    }

    if (rbuffer.count == 0){
        pthread_mutex_unlock(&rbuffer.lock);
        return RING_ERR_EMPTY;
    }

    void *slot = &rbuffer.packets[rbuffer.tail * PACKET_SIZE];
    memcpy(out_buffer, slot, PACKET_SIZE);
    rbuffer.tail = (rbuffer.tail + 1) % rbuffer.capacity;
    rbuffer.count--;
    pthread_mutex_unlock(&rbuffer.lock);

    return RING_SUCCESS;
}

uint8_t ring_buffer_count()
{
    return rbuffer.count;
}