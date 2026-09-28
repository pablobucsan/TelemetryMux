
#define _POSIX_C_SOURCE 200112L

#define NSEC_PER_SEC 1000000000L
#include "../../include/common/ring_buffer.h"
#include "../../include/common/packet.h"
#include "../../include/entities/producer.h"
#include "../../include/entities/consumer.h"
#include "../../include/common/stats.h"


#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>



uint64_t producer_write_times = 1000;
uint64_t consumer_read_times = 1000;

void *prod_thread_f(void *arg)
{
    producer_args *prod_args = (producer_args *)arg;

    /** Calculate interval in ns */
    uint64_t interval_ns = 1000000000ULL/prod_args->freq_hz;

    struct timespec next_wake;
    clock_gettime(CLOCK_MONOTONIC, &next_wake);

    for (uint64_t i = 0; i < producer_write_times; i++){
        prod_tick(prod_args->prod);

        /** Advance next wake time deterministically by interval */
        next_wake.tv_nsec += interval_ns;
        while (next_wake.tv_nsec >= NSEC_PER_SEC){
            next_wake.tv_sec += 1;
            next_wake.tv_nsec -= NSEC_PER_SEC;
        }

        /** 
         * Sleep until the exact calculated wake-up timestamp 
         * "Wake me up when the clock hits exactly this absolute timestamp"
         * */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_wake, NULL);

    }
    return NULL;
}

void *consumer_thread_f(void *arg)
{
    consumer_args *cons_args = (consumer_args *)arg;

    /** Calculate interval in ns */
    uint64_t interval_ns = 1000000000ULL/cons_args->freq_hz;

    struct timespec next_wake;
    clock_gettime(CLOCK_MONOTONIC, &next_wake);

    for (uint64_t i = 0; i < consumer_read_times; i++){
        consumer_tick(cons_args->cons);

        /** Advance next wake time deterministically by interval */
        next_wake.tv_nsec += interval_ns;
        while (next_wake.tv_nsec >= NSEC_PER_SEC){
            next_wake.tv_sec += 1;
            next_wake.tv_nsec -= NSEC_PER_SEC;
        }

        /** 
         * Sleep until the exact calculated wake-up timestamp 
         * "Wake me up when the clock hits exactly this absolute timestamp"
         * */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_wake, NULL);
    }
    return NULL;
}


int main()
{

    /** set up stats */
    stats_init();

    /** set up ring */
    uint8_t capacity = 128;
    ring_buffer_init(capacity);

    /** Producers and consumers */
    const uint64_t nprod = 1;
    const uint64_t ncons = 2;
    pthread_t thr[ncons + nprod];

    /** Assign frequencies for producers */
    uint32_t prod_freqs[3] = {200,250,150};
    producer prod[nprod];
    producer_args prod_args[nprod];

    for (uint64_t i = 0; i < nprod; i++){
        prod[i].producer_id = i;
        prod[i].sqn_number = 0;

        prod_args[i].prod = &prod[i];
        prod_args[i].freq_hz = prod_freqs[i];

        pthread_create(&thr[i], NULL, prod_thread_f, &prod_args[i]);

    }

    /** Assign frequencies for consumers */
    uint32_t cons_freqs[2] = {200,250};
    consumer cons[ncons];
    consumer_args cons_args[ncons];

    for (uint64_t i = 0; i < ncons; i++){
        cons[i].consumer_id = i;
        cons[i].sqn_number = 0;

        cons_args[i].cons = &cons[i];
        cons_args[i].freq_hz = cons_freqs[i];

        pthread_create(&thr[nprod + i], NULL, consumer_thread_f, &cons_args[i]);

    }


    /** Join threads */
    for (uint64_t i = 0; i < nprod + ncons; i++){
        pthread_join(thr[i], NULL);
    }



    uint64_t total_written = stats_get_total_written();
    uint64_t total_read = stats_get_total_read();
    uint64_t failed_write_busy = stats_get_failed_write_busy();
    uint64_t failed_write_full = stats_get_failed_write_full();

    uint64_t failed_read_busy = stats_get_failed_read_busy();
    uint64_t failed_read_empty = stats_get_failed_read_empty();

    uint8_t rbuffer_count = ring_buffer_count();
    /** Print stats */
    printf("STATS: \n");
    printf("TOTAL WRITTEN: %lu\n", total_written);
    printf("TOTAL READ: %lu\n", total_read);
    printf("FAILED WRITE BUSY: %lu\n", failed_write_busy);
    printf("FAILED WRITE FULL: %lu\n", failed_write_full);
    printf("FAILED READ BUSY: %lu\n", failed_read_busy);
    printf("FAILED READ EMPTY: %lu\n", failed_read_empty);
    printf("IN-RING COUNT: %hhu\n", rbuffer_count);

    assert(total_written == total_read + rbuffer_count);
    assert(total_written + failed_write_busy + failed_write_full == producer_write_times * nprod);
    assert(total_read + failed_read_busy + failed_read_empty == consumer_read_times * ncons);

    return 0;
}