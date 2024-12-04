#include "../headers/headers.h"

// #define _ZEPHYR__VERBOSE_
#include <semaphore.h>

THREAD_POOL(2);

#define NUM_THREADS 2
#define STACK_SIZE 512
#define NUM_OF_LOOPS 200

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

volatile int loop_counter = 1;
volatile int step_counter = 0;

// suspension_t so = SUSPENSION_INITIALIZER; // FIXME -> REMOVE ONLY USED IN M2

sem_t sem;

/*
 * Metodos que necesitaremos usar de semaforos
 *  -> sem_init?
 *  -> sem_wait
 *  -> sem_post
 * */

//********//
// TASK-H //
//********//

void *
task_h()
{
  sem_wait(&sem);
  while (1)
  {
#ifdef _ZEPHYR__VERBOSE_
    print_console("Task H: ");
    print_console("step_counter-> ");
    print_console_int(step_counter);
    print_console_newline();
#endif //_ZEPHYR__VERBOSE_

    // print_console ("SP: ");
    // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
    // print_console_newline ();

    if (loop_counter <= NUM_OF_LOOPS / 2)
    {
      measurements_hires__end_measurement();
    }
    tests_reports__assert(step_counter == 2);
    step_counter++;
    if (loop_counter > NUM_OF_LOOPS / 2)
    {
      measurements_hires__start_measurement();
    }
    // suspend_until_true (&so);
    sem_wait(&sem);
  }
}

//********//
// TASK-L //
//********//

struct timespec eat_time = TS(0, 10000000);
struct timespec next_time = TS(0, 0);

void *
task_l()
{

  while (1)
  {
#ifdef _ZEPHYR__VERBOSE_
    print_console("Task L1: ");
    print_console("step_counter-> ");
    print_console_int(step_counter);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_

    // print_console ("SP: ");
    // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
    // print_console_newline ();

    tests_reports__assert(step_counter == 0);
    step_counter++;

    sem_post(&sem); // activate task h
    // suspension_set_true (&so); // activate task h //FIXME -> REMOVE ONLY USED IN M2

    tests_reports__eat(eat_time);

#ifdef _ZEPHYR__VERBOSE_
    print_console("Task L2: ");
    print_console("step_counter-> ");
    print_console_int(step_counter);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_

    // print_console ("SP: ");
    // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
    // print_console_newline ();

    tests_reports__assert(step_counter == 1);
    step_counter++;

    if (loop_counter <= NUM_OF_LOOPS / 2)
    {
      measurements_hires__start_measurement();
    }

    k_yield();
    // yield_to_higher (); //FIXME -> REMOVE ONLY USED IN M2 -> NEED EQUIVALENT IN ZEPHYR

    if (loop_counter > NUM_OF_LOOPS / 2)
    {
      measurements_hires__end_measurement();
    }
#ifdef _ZEPHYR__VERBOSE_
    print_console("Task L3: ");
    print_console("step_counter-> ");
    print_console_int(step_counter);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_
    // print_console ("SP: ");
    // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
    // print_console_newline ();

    tests_reports__assert(step_counter == 3);
    step_counter++;

    tests_reports__eat(eat_time);

    if (loop_counter == NUM_OF_LOOPS / 2)
    {
      print_console("Measurement Preempt(ns): ");
      measurements_hires__print_measures_data_serial();
      measurements_hires__reset();
    }
    else if (loop_counter == NUM_OF_LOOPS)
    {
      print_console("Measurement EndPreempt(ns): ");
      measurements_hires__finish();
      tests_reports__test_ok();
    }
    loop_counter++;

#ifdef _ZEPHYR__VERBOSE_
    print_console("Loop Counter: ");
    print_console_int(loop_counter);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_
    step_counter = 0;

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
  }
}

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t thread_h, thread_l;
  pthread_attr_t attr_h, attr_l;
  struct sched_param sch_param_h = {.sched_priority = 5};
  struct sched_param sch_param_l = {.sched_priority = 4};

  m2osinit();
  check_posix_api();
  console_init(115200);

  // init measurements
  measurements_hires__init();

  sem_init(&sem, 0, 0);

  print_console("\nCS_Yield_To_Higher\n");
  print_console("\nMain starts\n");
  // CODE HERE

  pthread_attr_init(&attr_h);
  pthread_attr_init(&attr_l);

  // Stack size
  pthread_attr_setstacksize(&attr_h, STACK_SIZE);
  pthread_attr_setstacksize(&attr_l, STACK_SIZE);

  // Politica de planificación
  rc = pthread_attr_setschedpolicy(&attr_h, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  rc = pthread_attr_setschedpolicy(&attr_l, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");

  // Prioridades
  rc = pthread_attr_setschedparam(&attr_h, &sch_param_h);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setschedparam(&attr_l, &sch_param_l);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");

  // Thread stack
  rc = pthread_attr_setstack(&attr_h, thread_stack[0], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  rc = pthread_attr_setstack(&attr_l, thread_stack[1], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");

  // Create threads
  rc = pthread_create(&thread_h, &attr_h, task_h, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&thread_l, &attr_l, task_l, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  /*
   DIO.Put_Line ("TCB'Size" & Integer'Image (M2.Kernel.TCBs.TCB'Size));
   DIO.Put_Line ("System.Thread_T_Size_In_Bytes" &
                   Integer'Image (System.Thread_T_Size_In_Bytes));
   Tests_Reports.Assert
     (M2.Kernel.TCBs.TCB'Size = System.Thread_T_Size_In_Bytes * 8);*/

  // suspension_suspend_until_true (&so); //FIXME -> REMOVE ONLY USED IN M2

  // join thread
  pthread_join(thread_h, NULL);
  pthread_join(thread_l, NULL);
  print_console("\nMain ends\n");
  return 0;
}
