#ifndef _ZEPHYR_POSIX_TEST_HEADERS
#define _ZEPHYR_POSIX_TEST_HEADERS

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <zephyr/timing/timing.h>
#include <zephyr/kernel.h>

static inline void m2osinit()
{
}

static inline void console_init(int baudrate)
{
}

#endif // !_ZEPHYR_POSIX_TEST_HEADERS