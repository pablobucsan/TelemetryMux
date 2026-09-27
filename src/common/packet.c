

#include "../../include/common/packet.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



/**
 * @brief Serializes a Packet. Writes into the buffer all the packet fields as 
 * well as calculates the checksum and writes it into the buffer
 * 
 * @param pkt A ```NON-NULL``` pointer to the packet to serialize
 * @param buffer A ```NON-NULL``` pointer to the buffer to write into
 */
void serialize_packet(packet *pkt, uint8_t *buffer)
{
    buffer[0] = pkt->magic;
    buffer[1] = pkt->producer_id;

    memcpy(&buffer[2], &pkt->sqn_number, 2);

    buffer[4] = pkt->flags;

    memcpy(&buffer[5], &pkt->primary_payload, 3);
    memcpy(&buffer[8], &pkt->secondary_payload, 6);

    buffer[14] = pkt->reserved;

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 15; i++){
        checksum += buffer[i];
    }

    buffer[15] = checksum;
}


/**
 * @brief Deserializes a Packet. Validates its integrity and if correct, writes 
 * into the out_pkt all the fields
 * 
 * @param buffer A ```NON-NULL``` pointer to the buffer contaning the packet 
 * to read from
 * @param out_pkt A ```NON-NULL``` pointer to the packet to who's gonna be populated
 * 
 * @returns 0 on invalid packet, 1 otherwise
 */
uint8_t unpack_and_validate(uint8_t *buffer, packet *out_pkt)
{
    /** Verify magic */
    if (buffer[0] != PACKET_MAGIC){
        return 0;
    }

    /** Verify reserved */
    if (buffer[14] != 0){
        return 0;
    }

    /** Verify checksum */
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 15; i++){
        checksum += buffer[i];
    }
    if (checksum != buffer[15]){
        return 0;
    }

    /** Deserialize fields safely */
    out_pkt->magic = buffer[0];
    out_pkt->producer_id = buffer[1];

    memcpy(&out_pkt->sqn_number, &buffer[2], 2);

    out_pkt->flags = buffer[4];

    memcpy(&out_pkt->primary_payload, &buffer[5], 3);
    memcpy(&out_pkt->secondary_payload, &buffer[8], 6);

    out_pkt->reserved = buffer[14];
    out_pkt->checksum = buffer[15];

    return 1;
}


void test_print_pkt(packet *pkt)
{
    printf("------------------------\n");
    printf("MAGIC: 0x%x\n", pkt->magic);
    printf("PRODUCER ID: %hhu\n", pkt->producer_id);
    printf("SQN NUMBER: %hu\n", pkt->sqn_number);
    printf("FLAGS: 0x%x\n", pkt->flags);
    printf("PRIMARY: [");
    for (uint8_t i = 0; i < 2; i++){
        printf("%hhu, ", pkt->primary_payload[i]);
    }
    printf("%hhu]\n", pkt->primary_payload[2]);
    printf("SECONDARY: [");
    for (uint8_t i = 0; i < 5; i++){
        printf("%hhu, ", pkt->secondary_payload[i]);
    }
    printf("%hhu]\n", pkt->secondary_payload[5]);
    printf("RESERVED: %hhu\n", pkt->reserved);
    printf("------------------------\n");

}