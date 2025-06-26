#ifndef _M2_POSIX_TEST_HEADERS
#define _M2_POSIX_TEST_HEADERS
// #define _M2_VERBOSE_POSIX_TEST_
#include "../reports/tests_reports.h"
#include <pthread.h>
#include <stdint.h>
#include <m2_api.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <suspension.h>
extern uint32_t get_systick_val ();
extern uint32_t get_systick_reload();
extern uint32_t get_dwt_cyccnt ();
extern uint32_t get_dwt_cyccnt2 ();
extern void enable_dwt_cyccnt ();

// Empty functions to avoid compilation errors and facilitate test potability

typedef struct k_thread *k_tid_t; // Zephyr thread type definition

extern void m2osinit();
extern void check_posix_api();
extern void console_init(int baudrate);
extern void print_console(char *msg);
extern void print_console_int(int num);
extern void print_console_hex(uint32_t num);
extern void print_console_newline();

static inline k_tid_t
k_current_get ()
{
  return NULL;
}


static inline void
k_thread_runtime_stats_enable (k_tid_t thread)
{
}

static inline void
k_thread_runtime_stats_disable (k_tid_t thread)
{
}

#endif // _M2_POSIX_TEST_HEADERS
