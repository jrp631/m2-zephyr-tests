#include "../headers/headers.h"

THREAD_POOL (6);


#define NUM_OF_THEADS 6
#define PRIO 3
#define PERIOD_NS 100000000 // -> 0,1 seg

bool task1_executed = false;
struct timespec period_1 = TS (0, PERIOD_NS);
bool task2_executed = false;
struct timespec period_2 = TS (0, PERIOD_NS);
bool task3_executed = false;
struct timespec period_3 = TS (0, PERIOD_NS);
bool task4_executed = false;
struct timespec period_4 = TS (0, PERIOD_NS);
bool task5_executed = false;
struct timespec period_5 = TS (0, PERIOD_NS);
bool task6_executed = false;
struct timespec period_6 = TS (0, PERIOD_NS);

//*********//
//  task1  //
//*********//
struct timespec next_time1;

void *
task1 ()
{
  print_console ("Task 1\n");
  task1_executed = true;
  TS_INC (next_time1, period_1);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time1, NULL);
}

//*********//
//  task2  //
//*********//
struct timespec next_time2;

void *
task2 ()
{
  print_console ("Task 2\n");
  task2_executed = true;
  TS_INC (next_time2, period_2);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time2, NULL);
}

//*********//
//  task3  //
//*********//
struct timespec next_time3;

void *
task3 ()
{
  print_console ("Task 3\n");
  task3_executed = true;
  TS_INC (next_time3, period_3);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time3, NULL);
}

//*********//
//  task4  //
//*********//
struct timespec next_time4;

void *
task4 ()
{
  print_console ("Task 4\n");
  task4_executed = true;
  TS_INC (next_time4, period_4);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time4, NULL);
}

//*********//
//  task5  //
//*********//
struct timespec next_time5;

void *
task5 ()
{
  print_console ("Task 5\n");
  task5_executed = true;
  TS_INC (next_time5, period_5);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time5, NULL);
}

//*********//
//  task6  //
//*********//
struct timespec next_time6;

void *
task6 ()
{
  print_console ("Task 6\n");
  if (task6_executed)
    { // Second activation
      tests_reports__assert (task1_executed && task2_executed && task3_executed
                             && task4_executed && task5_executed);
      tests_reports__test_ok ();
    }
  else // First activation
    {
      tests_reports__assert (!task1_executed && !task2_executed
                             && !task3_executed && !task4_executed
                             && !task5_executed);
    }
  task6_executed = true;
  TS_INC (next_time6, period_6);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time6, NULL);
}

int
main (int argc, char const *argv[])
{

  pthread_t thread1, thread2, thread3, thread4, thread5, thread6;
  pthread_attr_t attr1, attr2, attr3, attr4, attr5, attr6;
  struct sched_param sch_param1 = { .sched_priority = PRIO };
  struct sched_param sch_param2 = { .sched_priority = PRIO };
  struct sched_param sch_param3 = { .sched_priority = PRIO };
  struct sched_param sch_param4 = { .sched_priority = PRIO };
  struct sched_param sch_param5 = { .sched_priority = PRIO };
  struct sched_param sch_param6 = { .sched_priority = PRIO + 1};

  clock_gettime (CLOCK_MONOTONIC, &next_time1);
  next_time2 = next_time1;
  next_time3 = next_time1;
  next_time4 = next_time1;
  next_time5 = next_time1;
  next_time6 = next_time1;

  m2osinit ();
  check_posix_api ();
  console_init (115200);

  print_console ("\nSize_Periodic_Task_Six_Test\n");
  print_console ("\nMain starts\n");

  // TEST CODE
  pthread_attr_init (&attr1);
  pthread_attr_setschedparam (&attr1, &sch_param1);
  pthread_attr_init (&attr2);
  pthread_attr_setschedparam (&attr2, &sch_param2);
  pthread_attr_init (&attr3);
  pthread_attr_setschedparam (&attr3, &sch_param3);
  pthread_attr_init (&attr4);
  pthread_attr_setschedparam (&attr4, &sch_param4);
  pthread_attr_init (&attr5);
  pthread_attr_setschedparam (&attr5, &sch_param5);
  pthread_attr_init (&attr6);
  pthread_attr_setschedparam (&attr6, &sch_param6);

  pthread_create (&thread1, &attr1, task1, NULL);
  pthread_create (&thread2, &attr2, task2, NULL);
  pthread_create (&thread3, &attr3, task3, NULL);
  pthread_create (&thread4, &attr4, task4, NULL);
  pthread_create (&thread5, &attr5, task5, NULL);
  pthread_create (&thread6, &attr6, task6, NULL);

  print_console ("\nMain ends");
  return 0;
}
