#include "../headers/headers.h"

THREAD_POOL (2);

const struct timespec period1 = TS (0, 0);
const struct timespec period2 = TS (0, 0);

// task 1 vars
int loop_counter_1 = 0;
struct timespec next_time1;

// task 2 vars
int loop_counter_2 = 0;
const int NUM_OF_LOOPS = 500;
struct timespec next_time2;

void *task1 ();
void *task2 ();

int
main (int argc, char const *argv[])
{
  pthread_t thread1, thread2;
  pthread_attr_t attr1, attr2;
  m2osinit ();
  check_posix_api ();
  console_init (115200);

  struct sched_param sch_param = { .sched_priority = 5 };
  clock_gettime (CLOCK_MONOTONIC, &next_time1);

  next_time2 = next_time1;

  print_console ("\nCS_Delay_Past_Time\n");
  print_console ("NUM_OF_LOOPS: ");
  print_console_int (NUM_OF_LOOPS);
  print_console ("\nMain starts\n");
  // TEST CODE

  pthread_attr_init (&attr1);
  pthread_attr_setschedparam (&attr1, &sch_param);
  pthread_attr_init (&attr2);
  pthread_attr_setschedparam (&attr2, &sch_param);

  pthread_create (&thread1, &attr1, task1, NULL);
  pthread_create (&thread2, &attr2, task2, NULL);

  measurements_hires__init ();

  print_console ("\nMain ends\n");
  return 0;
}

int cs_time;
void *
task1 ()
{
  if (loop_counter_2 > 0) // for some reason task2 starts before task1 and we
                          // need to consider that
    {
      measurements_hires__end_measurement ();
      // measurements_hires__print_measures_data (); // remove this line
    }
  if (loop_counter_1 == NUM_OF_LOOPS)
    {

      tests_reports__assert_equal_c (NUM_OF_LOOPS, loop_counter_2, 1);
      print_console (" CSTIME(ticks)= ");
      // cs_time = (int)(measurements_hires__measurement_time () * 100
      //                 / (loop_counter_1 + loop_counter_2) * 10);
      cs_time = (int)(measurements_hires__measurement_time ());
      print_console_int (cs_time);
      print_console_newline ();
      measurements_hires__finish ();
      tests_reports__test_ok ();
    }
  loop_counter_1++;
#ifdef _M2_VERBOSE_POSIX_TEST_
  print_console ("Start measurements task1 ....");
#endif // _M2_VERBOSE_POSIX_TEST_
  measurements_hires__start_measurement ();
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &period1, NULL);
}

void *
task2 ()
{
  if (loop_counter_1 > 0) // for some reason task2 starts before task1 and we
                          // need to consider that
    {
      measurements_hires__end_measurement ();
      // measurements_hires__print_measures_data();
      // measurements_hires__print_measures_data_serial (); // remove this line
    }
  loop_counter_2++;
  measurements_hires__start_measurement ();
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &period1, NULL);
}
