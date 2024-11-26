#include "../headers/headers.h"

THREAD_POOL (2);

#define NUM_OF_LOOPS 200

volatile int loop_counter = 1;
volatile int step_counter = 0;

suspension_t so = SUSPENSION_INITIALIZER;

//********//
// TASK-H //
//********//

void *
task_h ()
{
  // print_console ("Task H: ");
  // print_console ("step_counter-> ");
  // print_console_int (step_counter);
  // print_console_newline ();
  // print_console ("SP: ");
  // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
  // print_console_newline ();

  if (loop_counter <= NUM_OF_LOOPS / 2)
    {
      measurements_hires__end_measurement ();
    }
  tests_reports__assert (step_counter == 2);
  step_counter++;
  if (loop_counter > NUM_OF_LOOPS / 2)
    {
      measurements_hires__start_measurement ();
    }
  suspend_until_true (&so);
}

//********//
// TASK-L //
//********//

struct timespec eat_time = TS (0, 10000000);
struct timespec next_time = TS (0, 0);

void *
task_l ()
{

  // print_console ("Task L1: ");
  // print_console ("step_counter-> ");
  // print_console_int (step_counter);
  // print_console_newline ();

  // print_console ("SP: ");
  // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
  // print_console_newline ();

  tests_reports__assert (step_counter == 0);
  step_counter++;

  suspension_set_true (&so); // activate task h

  tests_reports__eat (eat_time);

  // print_console ("Task L2: ");
  // print_console ("step_counter-> ");
  // print_console_int (step_counter);
  // print_console_newline ();

  // print_console ("SP: ");
  // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
  // print_console_newline ();

  tests_reports__assert (step_counter == 1);
  step_counter++;

  if (loop_counter <= NUM_OF_LOOPS / 2)
    {
      measurements_hires__start_measurement ();
    }

  yield_to_higher ();

  if (loop_counter > NUM_OF_LOOPS / 2)
    {
      measurements_hires__end_measurement ();
    }

  // print_console ("Task L3: ");
  // print_console ("step_counter-> ");
  // print_console_int (step_counter);
  // print_console_newline ();

  // print_console ("SP: ");
  // print_console_hex ((uint32_t)m2_hal_regs_get_sp_reg ());
  // print_console_newline ();

  tests_reports__assert (step_counter == 3);
  step_counter++;

  tests_reports__eat (eat_time);

  if (loop_counter == NUM_OF_LOOPS / 2)
    {
      print_console ("Measurement Preempt(ns): ");
      measurements_hires__print_measures_data_serial ();
      measurements_hires__reset ();
    }
  else if (loop_counter == NUM_OF_LOOPS)
    {
      print_console ("Measurement EndPreempt(ns): ");
      measurements_hires__finish ();
      tests_reports__test_ok ();
    }
  loop_counter++;

  // print_console ("Loop Counter: ");
  // print_console_int (loop_counter);
  // print_console_newline ();

  step_counter = 0;

  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
}

int
main (int argc, char const *argv[])
{

  pthread_t thread_h, thread_l;
  pthread_attr_t attr_h, attr_l;
  struct sched_param sch_param_h = { .sched_priority = 5 };
  struct sched_param sch_param_l = { .sched_priority = 4 };

  m2osinit ();
  check_posix_api ();
  console_init (115200);
  print_console ("\nCS_Yield_To_Higher\n");
  print_console ("\nMain starts\n");
  // CODE HERE

  pthread_attr_init (&attr_h);
  pthread_attr_setschedparam (&attr_h, &sch_param_h);
  pthread_attr_init (&attr_l);
  pthread_attr_setschedparam (&attr_l, &sch_param_l);

  pthread_create (&thread_h, &attr_h, task_h, NULL);
  pthread_create (&thread_l, &attr_l, task_l, NULL);
  /*
   DIO.Put_Line ("TCB'Size" & Integer'Image (M2.Kernel.TCBs.TCB'Size));
   DIO.Put_Line ("System.Thread_T_Size_In_Bytes" &
                   Integer'Image (System.Thread_T_Size_In_Bytes));
   Tests_Reports.Assert
     (M2.Kernel.TCBs.TCB'Size = System.Thread_T_Size_In_Bytes * 8);*/

  // init measurements
  measurements_hires__init ();
  suspension_suspend_until_true (&so);

  print_console ("\nMain ends\n");
  return 0;
}
