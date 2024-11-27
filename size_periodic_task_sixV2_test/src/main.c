#include "../headers/headers.h"

THREAD_POOL (6);


#define NUM_OF_LOOPS 20 
#define PRIO 3
#define PERIOD_NS 20000000 // -> 0,02 seg

int loop_counter1 = 0;
struct timespec period_1 = TS (0, PERIOD_NS);
bool task2_executed = false;
int loop_counter2 = 0;
struct timespec period_2 = TS (0, PERIOD_NS*2);
bool task3_executed = false;
int loop_counter3 = 0;
struct timespec period_3 = TS (0, PERIOD_NS*3);
bool task4_executed = false;
int loop_counter4 = 0;
struct timespec period_4 = TS (0, PERIOD_NS*4);
bool task5_executed = false;
int loop_counter5 = 0;
struct timespec period_5 = TS (0, PERIOD_NS*5);
bool task6_executed = false;
int loop_counter6 = 0;
struct timespec period_6 = TS (0, PERIOD_NS*6);

//*********//
//  task1  //
//*********//
struct timespec next_time1;

void *
task1 ()
{
  print_console ("Task 1\n");
  if (loop_counter1 != 0) {
    print_console ("----------------\n");
    print_console ("   L1: ");
    print_console_int (loop_counter1);
    print_console ("\n");

    print_console ("   L2: ");
    print_console_int (loop_counter2);
    print_console ("\n");
    tests_reports__assert_equal_c (loop_counter2 * 2, loop_counter1, 1);

    print_console ("   L3: ");
    print_console_int (loop_counter3);
    print_console ("\n");
    tests_reports__assert_equal_c (loop_counter3 * 3, loop_counter1, 2);

    print_console ("   L4: ");
    print_console_int (loop_counter4);
    print_console ("\n");
    tests_reports__assert_equal_c (loop_counter4 * 4, loop_counter1, 3);

    print_console ("   L5: ");
    print_console_int (loop_counter5);
    print_console ("\n");
    tests_reports__assert_equal_c (loop_counter5 * 5, loop_counter1, 4);

    print_console ("   L6: ");
    print_console_int (loop_counter6);
    print_console ("\n");
    tests_reports__assert_equal_c (loop_counter6 * 6, loop_counter1, 5);

    print_console ("----------------\n");
  }

  if (loop_counter1 == NUM_OF_LOOPS) {
    tests_reports__test_ok ();
  }
  loop_counter1++;
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
  loop_counter2++;
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
  loop_counter3++;
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
  loop_counter4++;
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
  loop_counter5++;
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
  loop_counter6++;  
  TS_INC (next_time6, period_6);
  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time6, NULL);
}

int
main (int argc, char const *argv[])
{

  pthread_t thread1, thread2, thread3, thread4, thread5, thread6;
  pthread_attr_t attr1, attr2, attr3, attr4, attr5, attr6;
  struct sched_param sch_param1 = { .sched_priority = PRIO + 1 }; // PRIO TASK 1 (HIGHER PRIO)
  struct sched_param sch_param2 = { .sched_priority = PRIO };
  struct sched_param sch_param3 = { .sched_priority = PRIO };
  struct sched_param sch_param4 = { .sched_priority = PRIO };
  struct sched_param sch_param5 = { .sched_priority = PRIO };
  struct sched_param sch_param6 = { .sched_priority = PRIO };

  clock_gettime (CLOCK_MONOTONIC, &next_time1);
  next_time2 = next_time1;
  next_time3 = next_time1;
  next_time4 = next_time1;
  next_time5 = next_time1;
  next_time6 = next_time1;

  m2osinit ();
  check_posix_api ();
  console_init (115200);

  print_console ("\nSize_Periodic_Task_Sixv2_Test\n");
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
