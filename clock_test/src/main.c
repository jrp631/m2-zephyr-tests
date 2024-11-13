#include "../headers/headers.h"

#define NUM_OF_LOOPS 10000
struct timespec ts;
int loop_counter = 0;

int main(int argc, char **argv)
{
  m2osinit();
  console_init(115200);
  printf("\nMain starts\n");

  printf("Clock Test\n");
  printf("NUM_OF_LOOPS: %d\n", NUM_OF_LOOPS);

  measurements_hires__init();

  while (loop_counter < 20)
  {
    measurements_hires__start_measurement();
    for (int i = 0; i < NUM_OF_LOOPS; i++)
    {
      clock_gettime(CLOCK_MONOTONIC, &ts);
    }
    measurements_hires__end_measurement();
    loop_counter++;
    measurements_hires__finish();
  }

  printf("\nMain ends");
  return 0;
}
