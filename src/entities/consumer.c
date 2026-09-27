

#include "../../include/entities/consumer.h"
#include "../../include/common/packet.h"
#include "../../include/common/ring_buffer.h"
#include <stdio.h>


void consumer_tick(consumer *consumer)
{
    /** Read ring buffer, deseralize */

    /** Read ring buffer */
    uint8_t buffer[PACKET_SIZE];
    uint8_t result = ring_buffer_read(buffer);
    
    if (result == 0){
        printf("Consumer: Nothing to read from ring buffer\n");
        return;
    }
    
    /** Deserialize */
    packet pkt;
    uint8_t valid = unpack_and_validate(buffer, &pkt);
    if (valid == 0){
        printf("Consumer: Invalid packet\n");
        return;
    }

    /** For now, print it */
    test_print_pkt(&pkt);
}