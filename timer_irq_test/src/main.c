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

#define NUM_OF_LOOPS 20000
struct timespec avg_time_ts;
hwtime_t avg_time_hw;
float avg_time;
int num_of_longer_times = 0;
struct timespec sum_of_longer_times = {0, 0};

int main(int argc, char **argv)
{
#ifdef USE_M2
  m2osinit();
  console_init(115200);
#endif // USE_M2

  // puts("Main starts\n");
  printf("Timer_IRQ_Test\n");
  printf("NUM_OF_LOOPS: %d\n", NUM_OF_LOOPS);
  printf("Main starts\n");
  k_tid_t tid = k_current_get(); // FIXME
  //k_thread_runtime_stats_enable(tid);
  measurements_hires__init();

  // TODO IMPLEMENT TEST
  while (1)
  {

    // measure time to take measurements

    for (int i = 0; i < NUM_OF_LOOPS; i++)
    {
      measurements_hires__start_measurement();
      measurements_hires__end_measurement();
    }
    avg_time_ts = measurement_avg();
    hwtime_t measurement_time = TS2HWT(measurements_hires__get_measurement_time());
    avg_time = (double)measurement_time / (double)NUM_OF_LOOPS;

    printf("Avg Time: %lld s. %09ld ns\n", avg_time_ts.tv_sec, avg_time_ts.tv_nsec);
    printf("Time Units ");
    measurements_hires__print_data();

    // Detect times longer than expected
    for (int i = 0; i < NUM_OF_LOOPS / 30; i++)
    { // /30??
      measurements_hires__init();
      measurements_hires__start_measurement();
      measurements_hires__end_measurement();

      struct timespec measurement_time = measurements_hires__get_measurement_time();
      if (TS2HWT(measurement_time) > avg_time * 1.5) // FIXME
      {
        printf(" T:");
        struct timespec time = measurements_hires__get_measurement_time();
        printf(" %lld s. %09ld ns", time.tv_sec, time.tv_nsec);
        num_of_longer_times++;
        sum_of_longer_times.tv_sec += time.tv_sec;
        sum_of_longer_times.tv_nsec += time.tv_nsec;
      }
    }

    // Prints
    printf("\n");
    printf("Measurement Timer handler time:");
    // DIO.Put (Integer (Sum_Of_Longer_Times * Measurements.Time_Unit_In_Us) /
    //            Num_Of_Longer_Times - Integer (Avg_Time_Us));
    //hwitime_t dato =  (num_of_longer_times* ) - TS2HWT(avg_time_ts); // ?? dato???
    printf("TODO hwt\n");
    printf(" (Num of longer times: %d", num_of_longer_times);
    printf(" Sum of longer times: %lld s. %09ld ns", sum_of_longer_times.tv_sec, sum_of_longer_times.tv_nsec);
    printf(")\n");

    measurements_hires_finish();
  }

  //k_thread_runtime_stats_disable(tid); // FIXME

  // puts("Main ends\n");
  printf("\nMain ends\n");
  return 0;
}
