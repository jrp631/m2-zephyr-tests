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

void putts(struct timespec ts)
{
  printf("ts: %llds. %09ld ns\n", ts.tv_sec, ts.tv_nsec);
}

int main(int argc, char **argv)
{
#ifdef USE_M2
  m2osinit();
  console_init(115200);
#endif // USE_M2

  // puts("Main starts\n");
  printf("Main starts\n");

  printf("HWTIME_HZ: %d\n", HWTIME_HZ);
  // Test assignments and gets
  printf("Test assignments and gets\n");
  struct timespec ts = TS(35, 234000000);
  putts(ts);

  // test add
  printf("Test add\n");
  struct timespec ts1;
  struct timespec ts2 = TS(6, 511000000);
  struct timespec ts3 = TS(0, 234000000);
  TS_ADD(ts1, ts2, ts3);
  putts(ts1);
  if (ts1.tv_sec == 6 && ts1.tv_nsec == ts2.tv_nsec + ts3.tv_nsec)
  {
    printf("Test add OK\n");
  }
  else
  {
    printf("Test add FAIL\n");
  }
  // carry one seccond
  TS_ADD(ts3, ts2, ts1);
  putts(ts3);
  if (ts3.tv_sec == 13 && ts3.tv_nsec == 511000000 + 234000000 + 511000000 - 1000000000)
  {
    printf("Test add carry OK\n");
  }
  else
  {
    printf("Test add carry FAIL\n");
  }

  // test incr
  printf("Test incr\n");
  struct timespec ts1_old;
  struct timespec ts1_incr = TS(1, 0);
  struct timespec ts2_incr = TS(6, 511000000);
  ts1_old = ts1_incr;
  TS_INC(ts1_incr, ts2_incr);
  putts(ts1_incr);
  if (ts1_incr.tv_sec == 7 && ts1_incr.tv_nsec == 511000000)
  {
    printf("Test incr OK\n");
  }
  else
  {
    printf("Test incr FAIL\n");
  }
  ts1_old = ts1_incr;
  TS_INC(ts1_incr, ts2_incr);
  putts(ts1_incr);
  if (ts1_incr.tv_sec == 14 && ts1_incr.tv_nsec == 511000000 + 511000000 - 1000000000)
  {
    printf("Test incr OK\n");
  }
  else
  {
    printf("Test incr FAIL\n");
  }

  //test TS2HWT and HWT2TS
  printf("Test TS2HWT and HWT2TS\n");
  const int sec = 5;
  const int32_t nsec = 765000000;
  struct timespec ts_test = TS(sec, nsec);
  hwtime_t hwt = sec * HWTIME_HZ + ((uint64_t) nsec * HWTIME_HZ) / NS_IN_S;
  //printf("hwt: %d\n", hwt);
  //printf("ts_test 2 -> hwt: %lld\n", TS2HWT(ts_test2));
  
  if (ts_test.tv_sec == sec && ts_test.tv_nsec == nsec)
  {
    printf("Test HWT2TS OK\n");
  }
  else
  {
    printf("Test HWT2TS FAIL\n");
  }
  hwtime_t hwt2 = TS2HWT(ts_test);
  if (hwt == hwt2)
  {
    printf("Test TS2HWT OK\n");
  }
  else
  {
    printf("Test TS2HWT FAIL\n");
  }

  printf("\nMain ends\n");
  //const int sec_2 = 1;
  //const int32_t nsec_2 = 1000000000/2;
  //hwtime_t hwt_2 = sec_2 * HWTIME_HZ + ((uint64_t) nsec_2 * HWTIME_HZ) / NS_IN_S;

  return 0;
}
