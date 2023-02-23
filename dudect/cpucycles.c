#include "cpucycles.h"

#include <stdbool.h>

static bool stop = false;
static int64_t stop_time = 0;
static int64_t delta = 0;

static int64_t cpucycles_internal(void)
{
#if defined(__i386__) || defined(__x86_64__)
    unsigned int hi, lo;
    __asm__ volatile("rdtsc\n\t" : "=a"(lo), "=d"(hi));
    return ((int64_t) lo) | (((int64_t) hi) << 32);

#elif defined(__aarch64__)
    uint64_t val;
    /* According to ARM DDI 0487F.c, from Armv8.0 to Armv8.5 inclusive, the
     * system counter is at least 56 bits wide; from Armv8.6, the counter
     * must be 64 bits wide.  So the system counter could be less than 64
     * bits wide and it is attributed with the flag 'cap_user_time_short'
     * is true.
     */
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    return val;
#else
#error Unsupported Architecture
#endif
}

int64_t cpucycles(void)
{
    if (stop)
        return stop_time;
    return cpucycles_internal() + delta;
}

void cpucycles_start(void)
{
    if (!stop)
        return;
    delta += stop_time - cpucycles_internal();
    stop = false;
}

void cpucycles_stop(void)
{
    stop = true;
    stop_time = cpucycles_internal();
}
