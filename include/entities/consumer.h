

#ifndef CONSUMER_H
#define CONSUMER_H

#include <stdint.h>


typedef struct consumer{
    uint16_t sqn_number;
    uint8_t consumer_id;
}consumer;

void consumer_tick(consumer *consumer);

#endif //CONSUMER_H
