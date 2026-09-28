

#include "../../include/entities/consumer.h"
#include "../../include/common/packet.h"
#include "../../include/common/ring_buffer.h"
#include "../../include/common/stats.h"
#include <stdio.h>


void consumer_tick(consumer *consumer)
{
    /** Read ring buffer, deseralize */

    /** Read ring buffer */
    uint8_t buffer[PACKET_SIZE];
    uint8_t result = ring_buffer_read(buffer);
    
    switch(result){
        case RING_ERR_BUSY:{
            stats_incr_failed_read_busy();
            return;
        }
        case RING_ERR_EMPTY:{
            stats_incr_failed_read_empty();
            return;
        }
        case RING_SUCCESS:{
            stats_incr_total_read();
            break;
        }
        default:{
            return;
        }
    }
    
    /** Deserialize */
    packet pkt;
    uint8_t valid = unpack_and_validate(buffer, &pkt);
    if (valid == 0){
        printf("Consumer: Invalid packet\n");
        return;
    }

}