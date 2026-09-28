

#ifndef STATS_H
#define STATS_H


#include <stdint.h>

typedef struct stats{
    _Atomic uint64_t total_written;
    _Atomic uint64_t total_read;
    _Atomic uint64_t failed_write_busy;
    _Atomic uint64_t failed_write_full;
    _Atomic uint64_t failed_read_busy;
    _Atomic uint64_t failed_read_empty;
}stats;

void stats_init();
void stats_incr_total_written();
void stats_incr_total_read();
void stats_incr_failed_write_busy();
void stats_incr_failed_write_full();
void stats_incr_failed_read_busy();
void stats_incr_failed_read_empty();

uint64_t stats_get_total_written();
uint64_t stats_get_total_read();
uint64_t stats_get_failed_write_busy();
uint64_t stats_get_failed_write_full();
uint64_t stats_get_failed_read_busy();
uint64_t stats_get_failed_read_empty();


#endif //STATS_H