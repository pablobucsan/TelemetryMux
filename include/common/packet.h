
#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>


#define PACKET_MAGIC 0xAA
#define PACKET_SIZE 16

/**
 * @brief 16 byte structure describing an application message
 */
typedef struct packet{
    uint8_t magic;        
    uint8_t producer_id;
    uint16_t sqn_number;
    uint8_t flags;
    uint8_t primary_payload[3];
    uint8_t secondary_payload[6];
    uint8_t reserved;
    uint8_t checksum;
}packet;

void serialize_packet(packet *pkt, uint8_t *buffer);
uint8_t unpack_and_validate(uint8_t *buffer, packet *out_pkt);
void test_print_pkt(packet *pkt);

#endif //PACKET_H