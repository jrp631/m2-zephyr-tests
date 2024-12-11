#include "../headers/headers.h"

THREAD_POOL(2);

#define NUM_THREADS 2
#define STACK_SIZE 512

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

// TASKS

#define NUM_OF_LOOPS 20
#define PERIOD1_NS 2000000 // 20000000 // 200000000 // -> 0,2 seg //FIXME

const struct timespec period1 = TS(0, 0);

int loop_counter_1 = 0;
struct timespec start_time_t1;

int loop_counter_2 = 0;
bool other_task_executed = false;

//*******//
// TASK1 //
//*******//
struct timespec next_time1;
k_timeout_t period1_k = K_MSEC(0);
k_timeout_t next_time1_k = K_MSEC(2);

void *
task1()
{
  while (1)
  {
#ifdef _ZEPHYR__VERBOSE_
    print_console("Task1\n");
#endif //
    // clock_gettime(CLOCK_MONOTONIC, &next_time1);

    // if (loop_counter_1 != 0)
    // {
    //   tests_reports__assert(other_task_executed);
    // }
    if (loop_counter_1 == NUM_OF_LOOPS) // cs time
    {
#ifdef _ZEPHYR__VERBOSE_
      print_console("L1: ");
      print_console_int(loop_counter_1);
      print_console_newline();
#endif // _ZEPHYR__VERBOSE_
      print_console("  CSTime(ticks): ");
      print_console_int((int)measurements_hires__measurement_avg());
      print_console_newline();

      measurements_hires__finish();
      tests_reports__test_ok();
    }
    loop_counter_1++;
    other_task_executed = false;
    // TS_INC(next_time1, period1);
    // print_console("Start measurements task1 ....");
    measurements_hires__start_measurement();

    k_sleep(next_time1_k);

    // clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time1, NULL);
  }
}

//*******//
// TASK2 //
//*******//
struct timespec next_time_other = TS(0, 0);
k_timeout_t period_other_k = K_MSEC(0);

void *
other_task()
{
  while (1)
  {
    if (!other_task_executed)
    {
      measurements_hires__end_measurement();
      // print_console("... End measurements other task\n");
    }

#ifdef _ZEPHYR__VERBOSE_
    print_console("Other Task\n");
#endif // _ZEPHYR__VERBOSE_

    // tests_reports__assert(!other_task_executed);
    other_task_executed = true;
    k_sleep(period_other_k); // sleeps 0;
    // clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time_other, NULL);
  }
}

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t thread1, thread_other;
  pthread_attr_t attr1, attr_other;
  struct sched_param sch_param = {.sched_priority = 3};

  m2osinit();
  check_posix_api();
  console_init(115200);

  measurements_hires__init();

  print_console("\nCS_Delay_Simple\n");
  print_console("\nMain starts\n");
  // TEST CODE

  pthread_attr_init(&attr1);
  pthread_attr_init(&attr_other);

  // Stack size
  pthread_attr_setstacksize(&attr1, STACK_SIZE);
  pthread_attr_setstacksize(&attr_other, STACK_SIZE);

  // Scheduling policy
  rc = pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  rc = pthread_attr_setschedpolicy(&attr_other, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");

  // Priorities
  rc = pthread_attr_setschedparam(&attr1, &sch_param);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setschedparam(&attr_other, &sch_param);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");

  // Thread stack
  rc = pthread_attr_setstack(&attr1, thread_stack[0], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  rc = pthread_attr_setstack(&attr_other, thread_stack[1], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");

  // Create threads
  rc = pthread_create(&thread1, &attr1, task1, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&thread_other, &attr_other, other_task, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");

  // join threads
  pthread_join(thread1, NULL);
  pthread_join(thread_other, NULL);
  print_console("\nMain ends\n");
  return 0;
}
