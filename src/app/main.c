
#include "../../include/common/ring_buffer.h"
#include "../../include/common/packet.h"
#include "../../include/entities/producer.h"
#include "../../include/entities/consumer.h"
#include <stdint.h>
#include <stdio.h>


int main()
{
    /** set up ring */
    uint8_t capacity = 8;
    ring_buffer_init(capacity);


    /** Create entities */
    producer prod = {.producer_id = 1, .sqn_number = 0};
    consumer cons = {.consumer_id = 1, .sqn_number = 0};

    /** Have the producer tick */
    prod_tick(&prod);


    /** Have the consumer tick */
    consumer_tick(&cons);

    return 0;
}