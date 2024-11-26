#include "../headers/headers.h"


// FIXME
THREAD_POOL (1);

#define PERIOD 800000000 // -> 0,8 seg
const struct timespec period = TS (0, PERIOD);

//*******//
// TASK1 //
//*******//
struct timespec next_time;
int counter = 0;

void *
task ()
{
  // FIXME: DEBERÍA IR EN EL MAIN?
  // measurements_hires__init ();
  if (counter == 0)
    {
      measurements_hires__start_measurement ();
      clock_gettime (CLOCK_MONOTONIC, &next_time);
      //Print next_time: DEBUG ONLY
        // print_console ("Next time: ");
        // print_console_int (TS_SEC (next_time));
        // print_console (".");
        // print_console_int (TS_NSEC (next_time));  
        // print_console_newline ();
    }
  //

  while (1)
    {
      //print_console ("Task\n");
      // TODO: Completar
      if (counter == 1)
        {
          

          measurements_hires__end_measurement ();
          measurements_hires__finish ();
          
          //Get time and print it: DEBUG ONLY
            // clock_gettime (CLOCK_MONOTONIC, &next_time);
            // print_console ("Next time: ");
            // print_console_int (TS_SEC (next_time));
            // print_console (".");
            // print_console_int (TS_NSEC (next_time));
            // print_console_newline ();

          tests_reports__test_ok ();
        }
      counter++;
      TS_INC (next_time, period);

      clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
    }
}

int
main (int argc, char const *argv[])
{

  pthread_t thread;
  pthread_attr_t attr;
  struct sched_param sch_param = { .sched_priority = 3 };

  m2osinit ();
  check_posix_api ();
  console_init (115200);

  print_console ("\nDelay_And_Clock_Performance\n");
  // TEST CODE
  print_console ("\nMain\n");

  pthread_attr_init (&attr);
  pthread_attr_setschedparam (&attr, &sch_param);
  pthread_create (&thread, &attr, task, NULL);

  print_console ("\nMain ends\n");
  measurements_hires__init ();

  return 0;
}
