#include "../headers/headers.h"

THREAD_POOL (4);

#define PERIOD_NS 20000000 // -> 0,02 seg
#define NUM_OF_LOOPS 20

int loop_counter_task1 = 0;
int loop_counter_task2 = 0;
int loop_counter_task3 = 0;
int loop_counter_task4 = 0;

//*******//
// task1 //
//*******//

struct timespec next_time1;
struct timespec period1 = TS (0, PERIOD_NS);

void *
task1 (void *arg)
{
  clock_gettime (CLOCK_MONOTONIC, &next_time1);
  print_console ("Task1\n");

  if (loop_counter_task1 != 0)
    {
      print_console ("----------------\n");
      print_console ("   L1: ");
      print_console_int (loop_counter_task1);
      print_console ("\n");

      print_console ("   L2: ");
      print_console_int (loop_counter_task2);
      print_console ("\n");
      tests_reports__assert_equal_c (loop_counter_task2 * 2, loop_counter_task1,
                                     1);

      print_console ("   L3: ");
      print_console_int (loop_counter_task3);
      print_console ("\n");
      tests_reports__assert_equal_c (loop_counter_task3 * 3, loop_counter_task1,
                                     2);

      print_console ("   L4: ");
      print_console_int (loop_counter_task4);
      print_console ("\n");
      tests_reports__assert_equal_c (loop_counter_task4 * 4, loop_counter_task1,
                                     3);

      print_console ("----------------\n");
    }

  if (loop_counter_task1 == NUM_OF_LOOPS)
    {
      print_console ("L1: ");
      print_console_int (loop_counter_task1);
      print_console ("\n");

      print_console ("L2: ");
      print_console_int (loop_counter_task2);
      print_console ("\n");

      print_console ("L3: ");
      print_console_int (loop_counter_task3);
      print_console ("\n");

      print_console ("L4: ");
      print_console_int (loop_counter_task4);
      print_console ("\n");

      tests_reports__test_ok ();
    }
  loop_counter_task1++;
  TS_INC (next_time1, period1);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time1, NULL);

}

//*******//
// task2 //
//*******//

struct timespec next_time2;
struct timespec period2 = TS (0, PERIOD_NS * 2);

void *
task2 (void *arg)
{
  clock_gettime (CLOCK_MONOTONIC, &next_time2);
  print_console ("Task2\n");
  loop_counter_task2++;
  TS_INC (next_time2, period2);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time2, NULL);
}

//*******//
// task3 //
//*******//

struct timespec next_time3;
struct timespec period3 = TS (0, PERIOD_NS * 3);

void *
task3 (void *arg)
{
  clock_gettime (CLOCK_MONOTONIC, &next_time3);
  print_console ("Task3\n");
  loop_counter_task3++;
  TS_INC (next_time3, period3);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time3, NULL);

}

//*******//
// task4 //
//*******//

struct timespec next_time4;
struct timespec period4 = TS (0, PERIOD_NS * 4);

void *
task4 (void *arg)
{
  clock_gettime (CLOCK_MONOTONIC, &next_time4);
  print_console ("Task4\n");
  loop_counter_task4++;
  TS_INC (next_time4, period4);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time4, NULL);
}

int
main (int argc, char const *argv[])
{
  pthread_t thread1, thread2, thread3, thread4;
  pthread_attr_t attr1, attr2, attr3, attr4;
  struct sched_param sch_param
    = { .sched_priority = 3 }; // PRIO TASK 1 (HIGHER PRIO)
  struct sched_param sch_param2
    = { .sched_priority = 2 }; // PRIO TASK 2, 3, 4 (LOWER PRIO)

  m2osinit ();
  check_posix_api ();
  console_init (115200);

  print_console ("Test size_periodic_task_four_test\n");
  print_console ("Main start\n");
  // test code

  pthread_attr_init (&attr1);
  pthread_attr_init (&attr2);
  pthread_attr_init (&attr3);
  pthread_attr_init (&attr4);
  pthread_attr_setschedparam (&attr1, &sch_param);
  pthread_attr_setschedparam (&attr2, &sch_param2);
  pthread_attr_setschedparam (&attr3, &sch_param2);
  pthread_attr_setschedparam (&attr4, &sch_param2);

  pthread_create (&thread1, &attr1, task1, NULL);
  pthread_create (&thread2, &attr2, task2, NULL);
  pthread_create (&thread3, &attr3, task3, NULL);
  pthread_create (&thread4, &attr4, task4, NULL);

  print_console ("Main end\n");
  return 0;
}
