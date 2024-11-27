
#include "../headers/headers.h"

THREAD_POOL (2);

#define NUM_OF_LOOPS 20
#define PERIOD1_NS 20000000 // -> 0,02 seg
#define PERIOD2_NS 20000000 // -> 0,02 seg
const struct timespec period1 = TS (0, PERIOD1_NS);
const struct timespec period2 = TS (0, PERIOD2_NS * 2);
int loop_counter1 = 0;
int loop_counter2 = 0;

//****************//
//  thread_body1  //
//****************//
struct timespec next_activation_time1;

void *
thread_body1 (void *arg)
{
  print_console ("Thread 1\n");

  if (loop_counter1 != 0) // checks
    {
      print_console ("----------------\n");
      print_console ("   L1: ");
      print_console_int (loop_counter1);
      print_console ("\n");

      print_console ("   L2: ");
      print_console_int (loop_counter2);
      print_console ("\n");
      tests_reports__assert_equal_c (loop_counter2 * 2, loop_counter1, 1);

      print_console ("----------------\n");
    }

  if (loop_counter1 == NUM_OF_LOOPS) // Ends
    {
      tests_reports__test_ok ();
    }

  loop_counter1++;

  TS_INC (next_activation_time1, period1);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time1,
                   NULL);
}

//****************//
//  thread_body2  //
//****************//
struct timespec next_activation_time2 = TS (0, 0);

void *
thread_body2 (void *arg)
{
  print_console ("Thread 2\n");
  loop_counter2++;
  TS_INC (next_activation_time2, period2);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time2,
                   NULL);
}

//**********//
//   main   //
//**********//
int
main (int argc, char **argv)
{
  m2osinit ();
  console_init (115200);
  print_console ("Size_Periodic_Task_TwoV2\n");
  print_console ("Main\n");
  check_posix_api ();

  pthread_t th1;
  pthread_t th2;
  pthread_attr_t attr, attr2;
  struct sched_param sch_param = { .sched_priority = 3 };
  struct sched_param sch_param_2 = { .sched_priority = 2 };

  clock_gettime (CLOCK_MONOTONIC, &next_activation_time1);
  next_activation_time2 = next_activation_time1;

  pthread_attr_init (&attr);
  pthread_attr_setschedparam (&attr, &sch_param);
  pthread_attr_init (&attr2);
  pthread_attr_setschedparam (&attr2, &sch_param_2);

  pthread_create (&th1, &attr, thread_body1, NULL);
  pthread_create (&th2, &attr2, thread_body2, NULL);

  print_console ("Main ends\n");

  return 0;
}
