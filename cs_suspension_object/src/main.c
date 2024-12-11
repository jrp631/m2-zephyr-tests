#include "../headers/headers.h"
#include <semaphore.h>

THREAD_POOL(2);
#define NUM_THREADS 2
#define STACK_SIZE 512

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

#define NUM_OF_LOOPS 300 

sem_t sem1;
sem_t sem2;

int loop_counter1 = 0;
int loop_counter2 = 0;

//*******//
// task1 //
//*******//

void *task1()
{
  if (loop_counter1 == 0)
    measurements_hires__start_measurement();
  while (1)
  {
#ifdef _ZEPHYR__VERBOSE_
    print_console("Task 1, loop_counter: ");
    print_console_int(loop_counter1);
    print_console("\n");
#endif // _ZEPHYR__VERBOSE_

    if (loop_counter1 == NUM_OF_LOOPS)
    {
      measurements_hires__end_measurement();

      print_console("  L1: ");
      print_console_int(loop_counter1);
      print_console("\n");

      print_console("  L2: ");
      print_console_int(loop_counter2);
      print_console("\n");

      tests_reports__assert_equal_c(loop_counter2, loop_counter1, 0);

      print_console("  CSTIME(ns): ");
      print_console_int((int)measurements_hires__measurement_avg_ns() / (loop_counter1 + loop_counter2));
      print_console_newline();
      print_console("  CSTIME(ticks): ");
      print_console_int((int)measurements_hires__measurement_avg() / (loop_counter1 + loop_counter2));
      print_console_newline();
      print_console(" AVG: ");
      print_console_int((int)measurements_hires__measurement_avg());
      print_console_newline();
      measurements_hires__finish();
      tests_reports__test_ok();
    }

    loop_counter1++;

    if (loop_counter1 != 1)
      sem_post(&sem2);
    sem_wait(&sem1);
  }
}

//*******//
// task2 //
//*******//

void *task2()
{
  while (1)
  {
#ifdef _ZEPHYR__VERBOSE_
    print_console("Task 2 , loop counter: ");
    print_console_int(loop_counter2);
    print_console("\n");
#endif // _ZEPHYR__VERBOSE_
    // DIO.Put (Integer
    //(Ada.Real_Time.To_Duration (Ada.Real_Time.Clock)));

    loop_counter2++;

    sem_post(&sem1);
    // print_console("Task 2, sem_post sem1\n");
    sem_wait(&sem2);
    // print_console("Task 2, sem_wait sem2\n");

  }
}

int main(int argc, char **argv)
{
  pthread_t thread_task1, thread_task2;
  pthread_attr_t attr1, attr2;
  struct sched_param sch_param = {.sched_priority = 11};
  struct sched_param sch_param2 = {.sched_priority = 10};

  int rc;

  m2osinit();
  check_posix_api();

  console_init(115200);

  sem_init(&sem1, 0, 0);
  sem_init(&sem2, 0, 0);

  print_console("CS_Suspension_Object\n");
  print_console("Main start\n");
  // TEST CODE

  pthread_attr_init(&attr1);
  pthread_attr_init(&attr2);

  pthread_attr_setstacksize(&attr1, STACK_SIZE);
  pthread_attr_setstacksize(&attr2, STACK_SIZE);

  // Establecer la política de planificación a SCHED_FIFO
  rc = pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  }
  rc = pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  }

  // Establecer la prioridad de planificación
  rc = pthread_attr_setschedparam(&attr1, &sch_param);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_attr_setschedparam");
  }
  rc = pthread_attr_setschedparam(&attr2, &sch_param2);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_attr_setschedparam");
  }

  // Damos stack a los hilos
  rc = pthread_attr_setstack(&attr1, thread_stack[0], STACK_SIZE);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_attr_setstack");
  }
  rc = pthread_attr_setstack(&attr2, thread_stack[1], STACK_SIZE);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_attr_setstack");
  }

  // Crear los hilos
  rc = pthread_create(&thread_task1, &attr1, task1, NULL);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_create");
  }
  rc = pthread_create(&thread_task2, &attr2, task2, NULL);
  if (rc != 0)
  {
    handle_error_en(rc, "pthread_create");
  }

  measurements_hires__init();

  // measurements_hires__start_measurement();

  pthread_join(thread_task1, NULL);
  pthread_join(thread_task2, NULL);

  print_console("Main ends\n");
  return 0;
}
