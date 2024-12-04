#include "../headers/headers.h"

//THREAD_POOL(1);

#define NUM_OF_LOOPS 10000
struct timespec ts;
int loop_counter = 0;

int main(int argc, char **argv)
{
  m2osinit();
  console_init(115200);
  print_console("\nMain starts\n");

  print_console("Clock Test\n");
  print_console("NUM_OF_LOOPS: ");
  print_console_int(NUM_OF_LOOPS);
  print_console("\n");

  measurements_hires__init();

  while (loop_counter < 30)
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

  print_console("\nMain ends");
  tests_reports__test_ok(); // This is the last line of the main function
  return 0;
}
