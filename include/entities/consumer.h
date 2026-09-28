

#ifndef CONSUMER_H
#define CONSUMER_H

#include <stdint.h>


typedef struct consumer{
    uint16_t sqn_number;
    uint8_t consumer_id;
}consumer;

typedef struct consumer_args{
    consumer *cons;
    uint32_t freq_hz;
}consumer_args;

void consumer_tick(consumer *consumer);

#endif //CONSUMER_H
