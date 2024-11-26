#include "../headers/headers.h"

THREAD_POOL (2);

#define PERIOD1_NS 1000000 // 200000000 // -> 0,2 seg //FIXME
#define PERIOD2_NS 1000000 // 600000000 // -> 0,6 seg //FIXME

const struct timespec period1 = TS (0, 0);
const struct timespec period2 = TS (0, 0);

// task 1 vars
int loop_counter_1 = 0;
struct timespec next_time1;

// task 2 vars
int loop_counter_2 = 0;
const int NUM_OF_LOOPS = 300;
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
  print_console ("Task 1");
  print_console (" Loop_counter: ");
  print_console_int (loop_counter_1);
  print_console_newline ();
  measurements_hires__start_measurement ();
  if (loop_counter_1 == NUM_OF_LOOPS)
    {
      measurements_hires__end_measurement ();
      // print_console ("L1= ");
      // print_console_int (loop_counter_1);
      // print_console ("  L2=");
      // print_console_int (loop_counter_2);
      // print_console_newline ();

      // asset equal
      tests_reports__assert_equal_c (NUM_OF_LOOPS, loop_counter_2, 1);
      print_console (" CSTIME(ticks)= ");
      //cs_time = (int)(measurements_hires__measurement_time () * 100
      //                / (loop_counter_1 + loop_counter_2) * 10);
      cs_time = (int) (measurements_hires__measurement_time ());
      print_console_int (cs_time);
      print_console_newline ();
      measurements_hires__finish ();
      tests_reports__test_ok ();
    }
  loop_counter_1++;

  TS_INC (next_time1, period1);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time1, NULL);

  // delay until Ada.Real_Time.Time_First;
}

void *
task2 ()
{
  print_console ("Task 2");
  print_console (" Loop_counter: ");
  print_console_int (loop_counter_2);
  print_console_newline ();
  loop_counter_2++;
  TS_INC (next_time2, period2);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time2, NULL);
  // delay until Ada.Real_Time.Time_First;
}
