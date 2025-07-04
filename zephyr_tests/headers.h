#ifndef _ZEPHYR_POSIX_TEST_HEADERS
#define _ZEPHYR_POSIX_TEST_HEADERS

// #define _ZEPHYR__VERBOSE_

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <zephyr/posix/load.h>
#include <zephyr/posix/pthread.h>
#include <zephyr/posix/measurements.h>
#include <zephyr/kernel.h>
#include "../reports/tests_reports.h"

static inline void m2osinit()
{
}

static inline void check_posix_api()
{
}

static inline void console_init(int baudrate)
{
}

static inline void set_systick_reload(uint32_t reload)
{
}

//M2OS macro -> does nothing in Zephyr
#define THREAD_POOL(num_threads) \
   const int max_num_threads=num_threads; \

//m2os function renaming
#define print_console(msg) printf("%s", (msg))
#define print_console_int(num) printf("%"PRIu64, (uint64_t)(num))
#define print_console_newline() printf("\n")
#define measurements_hires__print_measures_data_serial() measurements_hires__print_measures_data()

#define handle_error_en(en, msg)                                                                   \
        do {                                                                                       \
                errno = en;                                                                        \
                perror(msg);                                                                       \
                exit(EXIT_FAILURE);                                                                \
        } while (0)


#endif // !_ZEPHYR_POSIX_TEST_HEADERS
