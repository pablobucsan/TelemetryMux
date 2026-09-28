

#ifndef PRODUCER_H
#define PRODUCER_H


#include <stdint.h>

typedef struct packet packet;

typedef struct producer{
    uint16_t sqn_number;
    uint8_t producer_id;
}producer;  

typedef struct producer_args{
    producer *prod;
    uint32_t freq_hz;
}producer_args;

void prod_tick(producer *prod);
void prod_make_packet(packet *out_pkt, producer *prod, uint8_t flags, uint8_t *prim, uint8_t *sec);


#endif //PRODUCER_H