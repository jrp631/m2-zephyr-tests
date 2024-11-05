#define USE_ZEPHYR 1

#ifdef USE_M2
#include "../reports/tests_reports.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
// FUNCIONES PARAM2
// TODO
extern uint32_t get_systick_val();
extern uint32_t get_dwt_cyccnt();
extern uint32_t get_dwt_cyccnt2();
extern void enable_dwt_cyccnt();
THREAD_POOL(1);

#endif // USE_M2

#ifdef USE_ZEPHYR
// COSAS ZEPHYR
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <zephyr/posix/load.h>
#include <zephyr/posix/pthread.h>
#include <zephyr/posix/measurements.h>
#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>

#endif // USE_ZEPHYR

#define NUM_OF_LOOPS 10000
struct timespec ts;
int loop_counter = 0;

int main(int argc, char **argv)
{
#ifdef USE_M2
  m2osinit();
  console_init(115200);
#endif // USE_M2

  // puts("Main starts\n");
  printf("Main starts\n");
  k_tid_t tid = k_current_get(); // FIXME
  k_thread_runtime_stats_enable(tid);

  printf("Clock Test\n");
  printf("NUM_OF_LOOPS: %d\n", NUM_OF_LOOPS);

  measurements_hires__init();

  while (1)
  {
    measurements_hires__start_measurement();
    for (int i = 0; i < NUM_OF_LOOPS; i++)
    {
      // TODO: READ REAL TIME CLOCK????
      clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
    }
    measurements_hires__end_measurement();
    loop_counter++;
    measurements_hires_finish();
  }

  k_thread_runtime_stats_disable(tid); // FIXME

  // puts("Main ends\n");
  printf("\nMain ends\n");
  return 0;
}
