#include "../headers/headers.h"

#define LOOPS 6

int64_t uptime_get[LOOPS];
uint32_t cycle_get_32[LOOPS];
uint64_t cycle_get_64[LOOPS];
uint32_t clock_cycle_get_32[LOOPS];
uint64_t clock_cycle_get_64[LOOPS];

int main(int argc, char **argv)
{
  m2osinit();
  console_init(115200);

  printf("\nMain starts\n");
/*
  // k_uptime_get
  printf("k_uptime_get...\n");
  for (int i = 0; i < LOOPS; i++)
  {
    if (LOOPS%2 == 0)
    {
      k_sleep(K_MSEC(1000));
    }
    uptime_get[i] = k_uptime_get();
  }
  // k_cycle_get_32
  printf("k_cycle_get_32...\n");
  for (int i = 0; i < LOOPS; i++)
  {
    if (LOOPS%2 == 0)
    {
      k_sleep(K_MSEC(1000));
    }
    cycle_get_32[i] = k_cycle_get_32();
  }
  // k_cycle_get_64
  printf("k_cycle_get_64...\n");
  for (int i = 0; i < LOOPS; i++)
  {
    if (LOOPS%2 == 0)
    {
      k_sleep(K_MSEC(1000));
    }
    cycle_get_64[i] = k_cycle_get_64();
  }
  // sys_clock_cycle_get_32
  printf("sys_clock_cycle_get_32...\n");
  for (int i = 0; i < LOOPS; i++)
  {
    if (LOOPS%2 == 0)
    {
      k_sleep(K_MSEC(1000));
    }
    clock_cycle_get_32[i] = sys_clock_cycle_get_32();
  }

  // sys_clock_cycle_get_64
  printf("sys_clock_cycle_get_64...\n");
  for (int i = 0; i < LOOPS; i++)
  {
    if (LOOPS%2 == 0)
    {
      k_sleep(K_MSEC(1000));
    }
    clock_cycle_get_64[i] = sys_clock_cycle_get_64();
  }

  // print the results int a table format
  printf("%-20s %-20s %-20s %-20s %-20s\n", "k_uptime_get", "k_cycle_get_32", "k_cycle_get_64", "sys_clock_cycle_get_32", "sys_clock_cycle_get_64");
  for (int i = 0; i < LOOPS; i++)
  {
    printf("Loop: %d\n", i);
    printf("%-20lld %-20u %-20lld %-20u %-20lld\n", uptime_get[i], cycle_get_32[i], cycle_get_64[i], clock_cycle_get_32[i], clock_cycle_get_64[i]);
  }
*/
  for (int i = 0; i < LOOPS; i++)
  {
    if(i == LOOPS/2){
      k_sleep(K_MSEC(30000)); // 25 seconds to check if the tick overflow
    }
    clock_cycle_get_64[i] = sys_clock_cycle_get_64();
  }
  for (int i = 0; i < LOOPS; i++)
  {
    printf("Loop: %d\n", i);
    printf("%-20lld\n", clock_cycle_get_64[i]);
  }
  printf("\nMain ends");
  return 0;
}
