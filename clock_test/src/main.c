#include "../headers/headers.h"

#define NUM_OF_LOOPS 10000
struct timespec ts;
int loop_counter = 0;

int main(int argc, char **argv)
{
  m2osinit();
  console_init(115200);
  k_tid_t tid = k_current_get();//FIXME
  //k_thread_runtime_stats_enable(tid);

  // puts("Main starts\n");
  printf("\nMain starts\n");

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
    measurements_hires__finish();
  }

  //k_thread_runtime_stats_disable(tid);//FIXME
  // puts("Main ends\n");
  printf("\nMain ends");
  return 0;
}
