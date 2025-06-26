#include "../headers/headers.h"

#define NUM_THREADS 2
#define STACK_SIZE 512

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);
 
THREAD_POOL(2);

#define PERIOD_IN_NS 1000000 // 1ms

const struct timespec period1 = TS(0, 0);
const struct timespec period2 = TS(0, 0);

k_timeout_t period1_k = K_MSEC(0);
k_timeout_t period2_k = K_MSEC(0);

// task 1 vars
int loop_counter_1 = 0;
struct timespec next_time1;

// task 2 vars
int loop_counter_2 = 0;
const int NUM_OF_LOOPS = 500;
struct timespec next_time2;

void *task1();
void *task2();

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t thread1, thread2;
  pthread_attr_t attr1, attr2;
  m2osinit();
  check_posix_api();
  console_init(115200);

  struct sched_param sch_param = {.sched_priority = 4};
  struct sched_param sch_param_2 = {.sched_priority = 4};

  measurements_hires__init();

  print_console("\nCS_Delay_Past_Time\n");
  print_console("\nMain starts\n");

  pthread_attr_init(&attr1);
  pthread_attr_init(&attr2);

  // Stack size
  pthread_attr_setstacksize(&attr1, STACK_SIZE);
  pthread_attr_setstacksize(&attr2, STACK_SIZE);

  // Scheduling policy
  rc = pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  rc = pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");

  // Priorities 
  rc = pthread_attr_setschedparam(&attr1, &sch_param);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setschedparam(&attr2, &sch_param_2);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");

  // Thread stack 
  rc = pthread_attr_setstack(&attr1, thread_stack[0], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  rc = pthread_attr_setstack(&attr2, thread_stack[1], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");

  clock_gettime(CLOCK_MONOTONIC, &next_time1);
  next_time2 = next_time1;
  // Create threads
  rc = pthread_create(&thread1, &attr1, task1, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create_task1");
  rc = pthread_create(&thread2, &attr2, task2, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create_task2");

  //join thread 
  rc = pthread_join(thread1, NULL);
  rc = pthread_join(thread2, NULL);

  print_console("\nMain ends\n");
  return 0;
}

int cs_time;
void *
task1()
{
  while (1)
  {
    if (loop_counter_1 > 0) {
      measurements_hires__end_measurement();
#ifdef _ZEPHYR__VERBOSE_
      print_console("... End measurements task1\n");
#endif // _ZEPHYR__VERBOSE_
    }

#ifdef _ZEPHYR__VERBOSE_
    print_console("Task 1");
    print_console(" Loop_counter: ");
    print_console_int(loop_counter_1);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_
    if (loop_counter_1 == NUM_OF_LOOPS)
    {
      // measurements_hires__end_measurement();
#ifdef _ZEPHYR__VERBOSE_
      print_console ("L1= ");
      print_console_int (loop_counter_1);
      print_console ("  L2=");
      print_console_int (loop_counter_2);
      print_console_newline ();
#endif // _ZEPHYR__VERBOSE_

      // asset equal
      tests_reports__assert_equal_c(NUM_OF_LOOPS, loop_counter_2, 1);
      print_console(" CSTIME(ticks)= ");
      // cs_time = (int)(measurements_hires__measurement_time () * 100
      //                 / (loop_counter_1 + loop_counter_2) * 10);
      cs_time = (int)(measurements_hires__measurement_time());
      print_console_int(cs_time);
      print_console_newline();
      measurements_hires__finish();
      tests_reports__test_ok();
    }
    loop_counter_1++;
#ifdef _ZEPHYR__VERBOSE_
    print_console("Start measurements task1 ...");
#endif // _ZEPHYR__VERBOSE_
    measurements_hires__start_measurement();
    // k_sleep(period1_k);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period1, NULL);
  }
}

void *
task2()
{
  while (1)
  { 
    measurements_hires__end_measurement();
#ifdef _ZEPHYR__VERBOSE_
    print_console("... End measurements task2\n");
    print_console("Task 2");
    print_console(" Loop_counter: ");
    print_console_int(loop_counter_2);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_
    loop_counter_2++;
    // TS_INC(next_time2, period2);
#ifdef _ZEPHYR__VERBOSE_
    print_console("Start measurements task2 ...");
#endif // _ZEPHYR__VERBOSE_
    measurements_hires__start_measurement();
    // k_sleep(period2_k);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period2, NULL);
  }
}
