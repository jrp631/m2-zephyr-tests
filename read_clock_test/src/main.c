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

#define NUM_OF_LOOPS 10
#define MEAS_PER_LOOP 100
#define EAT_LOOPS 1000
const struct timespec eat_ts_var = TS(0, 100000);

// extern uint64_t m2_hal_regs_get_cntvct();
// extern uint64_t m2_hal_regs_get_pmccntr();

#define NUM_OF_MEAS 20
uint32_t hwt[NUM_OF_MEAS];
uint32_t hwtv[NUM_OF_MEAS];

int main(int argc, char **argv)
{
#ifdef USE_M2
  m2osinit();
  console_init(115200);
#endif // USE_M2

  // puts("Main starts\n");
  printf("Main starts\n");
  k_tid_t tid = k_current_get();//FIXME
  k_thread_runtime_stats_enable(tid);
  measurements_hires__init();

  for (int i = 0; i < NUM_OF_LOOPS; i++)
  {
    measurements_hires__reset();
    for (int j = 0; j < MEAS_PER_LOOP; j++)
    {
      measurements_hires__start_measurement();
      for (int k = 0; k < EAT_LOOPS * i; k++)
      {
      }
      measurements_hires__end_measurement();
    }
    // putint(EAT_LOOPS * i);
    printf("%d", EAT_LOOPS * i);
    // puts(":");
    printf(":");
    // measurements_hires__print_measures_data();
    measurements_hires__print_data();
  }
  k_thread_runtime_stats_disable(tid);//FIXME

  // puts("Main ends\n");
  printf("\nMain ends\n");
  return 0;
}
