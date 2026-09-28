

#include "../../include/common/stats.h"
#include <stdatomic.h>

static stats st;

void stats_init()
{
    st.total_written = 0;
    st.total_read = 0;
    st.failed_write_busy = 0;
    st.failed_write_full = 0;
    st.failed_read_busy = 0;
    st.failed_read_empty = 0;
}

void stats_incr_total_written()
{
    atomic_fetch_add(&st.total_written, 1);
}

void stats_incr_total_read()
{
    atomic_fetch_add(&st.total_read, 1);
}

void stats_incr_failed_write_busy()
{
    atomic_fetch_add(&st.failed_write_busy, 1);
}

void stats_incr_failed_write_full()
{
    atomic_fetch_add(&st.failed_write_full, 1);
}

void stats_incr_failed_read_busy()
{
    atomic_fetch_add(&st.failed_read_busy, 1);
}

void stats_incr_failed_read_empty()
{
    atomic_fetch_add(&st.failed_read_empty, 1);
}

uint64_t stats_get_total_written()
{
    return st.total_written;
}

uint64_t stats_get_total_read()
{
    return st.total_read;
}

uint64_t stats_get_failed_write_busy()
{
    return st.failed_write_busy;
}

uint64_t stats_get_failed_write_full()
{
    return st.failed_write_full;
}

uint64_t stats_get_failed_read_busy()
{
    return st.failed_read_busy;
}

uint64_t stats_get_failed_read_empty()
{
    return st.failed_read_empty;
}




