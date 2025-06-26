#include "../headers/headers.h"

THREAD_POOL(2);

#define NUM_OF_LOOPS 300 //8 for arduino short time

suspension_t so1 = SUSPENSION_INITIALIZER;
suspension_t so2 = SUSPENSION_INITIALIZER;

int loop_counter1 = 0;
int loop_counter2 = 0;



//*******//
// task1 //
//*******//

void * task1() {
  if (loop_counter1 == 0)
    measurements_hires__start_measurement();
  
  if (loop_counter1 == NUM_OF_LOOPS)
  {
    measurements_hires__end_measurement();

    print_console ("  L1: ");
    print_console_int (loop_counter1);
    print_console ("\n");
    
    print_console ("  L2: ");
    print_console_int (loop_counter2);
    print_console ("\n");

    tests_reports__assert_equal_c (loop_counter2, loop_counter1, 0);

    print_console ("  CSTIME(ns): ");
    print_console_int ((int)measurements_hires__measurement_avg_ns () / (loop_counter2 + 2));
    print_console ("\n");
    print_console ("  CSTIME(ticks): ");
    print_console_int ((int)measurements_hires__measurement_avg () / (loop_counter2 + 2));
    print_console ("\n");
    print_console (" AVG:");
    print_console_int ((int)measurements_hires__measurement_avg ());
    print_console ("\n");

    measurements_hires__finish ();
    tests_reports__test_ok ();
  }
  

  loop_counter1++;

  suspension_set_true (&so2);
  suspension_suspend_until_true (&so1);
}

//*******//
// task2 //
//*******//

void * task2() {
  
  loop_counter2++;
  suspension_set_true (&so1);
  suspension_suspend_until_true (&so2);
}

int main(int argc, char **argv)
{
  pthread_t thread_task1, thread_task2;
  pthread_attr_t attr1, attr2;
  struct sched_param sch_param = { .sched_priority = 4 };
  struct sched_param sch_param2 = { .sched_priority = 3};


  m2osinit();
  check_posix_api ();

  console_init(115200);

  print_console("CS_Suspension_Object\n");
  print_console("Main start\n");
  //TEST CODE 

  pthread_attr_init(&attr1);
  pthread_attr_setschedparam(&attr1, &sch_param);
  pthread_attr_init(&attr2);
  pthread_attr_setschedparam(&attr2, &sch_param2);

  pthread_create(&thread_task1, &attr1, task1, NULL);
  pthread_create(&thread_task2, &attr2, task2, NULL);

  measurements_hires__init ();

  print_console ("Main ends\n");
  return 0;
}
