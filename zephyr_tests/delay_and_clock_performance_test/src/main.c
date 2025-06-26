#include "../headers/headers.h"

#define NUM_THREADS 1
#define STACK_SIZE 512

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

// FIXME
THREAD_POOL(1);

#define PERIOD 80000000 // -> 0,08 seg
#define NUM_OF_LOOPS 200
const struct timespec period = TS(0, PERIOD);

//*******//
// TASK1 //
//*******//
struct timespec next_time;
int counter = 0;

void *
task()
{
  int rc;
  while (counter < NUM_OF_LOOPS)
  {
    if (counter % 2 == 0)
    {
      // printf("Task : %d\n", counter);
      counter++;
      //clock_gettime(CLOCK_MONOTONIC, &next_time);
      //TS_INC(next_time, period);
      measurements_hires__start_measurement();
      k_sleep(K_MSEC(80));
      //rc = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
      //printf("RC : %d\n", rc);
    } else if (counter % 2 == 1)
    {
      measurements_hires__end_measurement();
      counter++;
      // printf("Task ends\n");
    }
  }
  measurements_hires__finish();
  tests_reports__test_ok();
}

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t thread;
  pthread_attr_t attr;
  struct sched_param sch_param = {.sched_priority = 3};

  m2osinit();
  check_posix_api();
  console_init(115200);

  print_console("\nDelay_And_Clock_Performance\n");
  // TEST CODE
  print_console("\nMain\n");

  measurements_hires__init();

  pthread_attr_init(&attr);

  pthread_attr_setstacksize(&attr, STACK_SIZE);

  rc = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");

  rc = pthread_attr_setschedparam(&attr, &sch_param);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setstack(&attr, thread_stack[0], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");

  rc = pthread_create(&thread, &attr, task, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create_task");

  pthread_join(thread, NULL);

  print_console("\nMain ends\n");

  return 0;
}
