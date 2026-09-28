

#include "../../include/entities/producer.h"
#include "../../include/common/packet.h"
#include "../../include/common/ring_buffer.h"
#include "../../include/common/stats.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>




void prod_tick(producer *prod)
{
    /** Make packets, serialize, write to ring buffer */

    /** Make the packet */
    packet pkt;
    uint8_t primary[3];
    uint8_t secondary[6];
    for (uint8_t i = 0; i < 3; i++){
        primary[i] = i;
    }
    for (uint8_t i = 0; i < 6; i++){
        secondary[i] = i;
    }

    prod_make_packet(&pkt, prod, 0xAA, primary, secondary);

    /** Serialize the packet */
    uint8_t buffer[PACKET_SIZE];
    serialize_packet(&pkt, buffer);

    /** Write to buffer */
    uint8_t result = ring_buffer_write(buffer);
    switch(result){
        case RING_ERR_BUSY:{
            stats_incr_failed_write_busy();
            break;
        }
        case RING_ERR_FULL:{
            stats_incr_failed_write_full();
            break;
        }
        case RING_SUCCESS:{
            stats_incr_total_written();
            break;
        }
        default:{
            break;
        }
    }
}


/**
 * @brief Makes a packet given the producer, flags, primary and secondary payload
 * 
 * @param out_pkt A ```NON-NULL``` pointer to the packet to populate
 * @param prod A ```NON-NULL``` pointer to the producer that is making this packet
 * @param flags Flags for the packet
 * @param prim A ```NON-NULL``` pointer to the stream containing the primary payload
 * @param sec A ```NON-NULL``` pointer to the stream containing the secondary payload
 * 
 */
void prod_make_packet(packet *out_pkt, producer *prod, uint8_t flags, uint8_t *prim, uint8_t *sec)
{
    memset(out_pkt, 0, sizeof(packet));

    out_pkt->magic = PACKET_MAGIC;
    out_pkt->producer_id = prod->producer_id;
    out_pkt->sqn_number = prod->sqn_number;
    out_pkt->flags = flags;
    memcpy(out_pkt->primary_payload, prim, 3);
    memcpy(out_pkt->secondary_payload, sec, 6);
    out_pkt->reserved = 0;
    /** Checksum is computed by serialize */
}



