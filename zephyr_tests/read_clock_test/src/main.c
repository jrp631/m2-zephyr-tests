#include "../headers/headers.h"

//THREAD_POOL (1);

#define NUM_OF_LOOPS 10
#define MEAS_PER_LOOP 100
#define EAT_LOOPS 1000
const struct timespec eat_ts_var = TS (0, 100000);

#define NUM_OF_MEAS 20
uint32_t hwt[NUM_OF_MEAS];
uint32_t hwtv[NUM_OF_MEAS];

int
main (int argc, char **argv)
{
  m2osinit ();
  console_init (115200);
  set_systick_reload (0xFFFFFF);
  print_console ("\nMain starts\n");
  measurements_hires__init ();

  for (int i = 0; i < NUM_OF_LOOPS; i++)
    {
      measurements_hires__reset ();
      for (int j = 0; j < MEAS_PER_LOOP; j++)
        {
          measurements_hires__start_measurement ();
          for (int k = 0; k < EAT_LOOPS * i; k++)
            {
            }
          measurements_hires__end_measurement ();
        }
      // putint(EAT_LOOPS * i);
      print_console_int (EAT_LOOPS * i);
      // puts(":");
      print_console (":");
      // measurements_hires__print_measures_data();
      measurements_hires__print_measures_data_serial ();
    }
  //  puts("Main ends\n");
  print_console ("\nMain ends");
  tests_reports__test_ok (); //FIXME
  return 0;
}
