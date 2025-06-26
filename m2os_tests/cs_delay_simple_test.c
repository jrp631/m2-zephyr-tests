#include "../headers/headers.h"

THREAD_POOL (2);

// TASKS

#define NUM_OF_LOOPS 500    // 20
#define PERIOD1_NS 20000000 // -> 20ms

const struct timespec period1 = TS (0, PERIOD1_NS);
const struct timespec period_other = TS (0, 0);
int loop_counter_1 = 0;
struct timespec start_time_t1;

int loop_counter_2 = 0;
bool other_task_executed = false;

//*******//
// TASK1 //
//*******//
struct timespec next_time1;

void *
task1 ()
{
  clock_gettime (CLOCK_MONOTONIC, &next_time1);

  if (loop_counter_1 == NUM_OF_LOOPS) // cs time
    {
      print_console ("  CSTime(ticks): ");
      print_console_int ((int)measurements_hires__measurement_avg ());
      print_console_newline ();
      measurements_hires__finish ();
      tests_reports__test_ok ();
    }
  loop_counter_1++;
  other_task_executed = false;
  TS_INC (next_time1, period1);
  measurements_hires__start_measurement ();
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time1, NULL);
}

//*******//
// TASK2 //
//*******//
struct timespec next_time_other = TS (0, 0);

void *
other_task ()
{
  if (!other_task_executed)
    {
      measurements_hires__end_measurement ();
    }

  other_task_executed = true;

  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time_other, NULL);
}

int
main (int argc, char const *argv[])
{
  pthread_t thread1, thread_other;
  pthread_attr_t attr1, attr_other;
  struct sched_param sch_param = { .sched_priority = 3 };

  m2osinit ();
  check_posix_api ();
  console_init (115200);

  print_console ("\nCS_Delay_Simple\n");
  print_console ("NUM_OF_LOOPS: ");
  print_console_int (NUM_OF_LOOPS);
  print_console ("\nMain starts\n");

  pthread_attr_init (&attr1);
  pthread_attr_setschedparam (&attr1, &sch_param);
  pthread_attr_init (&attr_other);
  pthread_attr_setschedparam (&attr_other, &sch_param);

  pthread_create (&thread_other, &attr_other, other_task, NULL);
  pthread_create (&thread1, &attr1, task1, NULL);

  measurements_hires__init ();

  print_console ("\nMain ends\n");
  return 0;
}
