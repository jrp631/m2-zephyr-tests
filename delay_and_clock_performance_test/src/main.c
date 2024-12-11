#include "../headers/headers.h"

#define NUM_THREADS 1
#define STACK_SIZE 512

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

// FIXME
THREAD_POOL(1);

#define PERIOD 800000000 // -> 0,8 seg
const struct timespec period = TS(0, PERIOD);

//*******//
// TASK1 //
//*******//
struct timespec next_time;
int counter = 0;

void *
task()
{
  // FIXME: DEBERÍA IR EN EL MAIN?
  // measurements_hires__init ();
  if (counter == 0)
  {
    measurements_hires__start_measurement();
    clock_gettime(CLOCK_MONOTONIC, &next_time);
    // Print next_time: DEBUG ONLY
#ifdef _ZEPHYR__VERBOSE_
    print_console("Initial time: ");
    print_console_int(next_time.tv_sec);
    print_console(".");
    print_console_int(next_time.tv_nsec);
    print_console_newline();
#endif // _ZEPHYR__VERBOSE_
  }
  //

  while (1)
  {
  #ifdef _ZEPHYR__VERBOSE_
    print_console ("Task\n");
  #endif // _ZEPHYR__VERBOSE_
    //  TODO: Completar
    if (counter == 1)
    {

      measurements_hires__end_measurement();
      measurements_hires__finish();

      // Get time and print it: DEBUG ONLY
#ifdef _ZEPHYR__VERBOSE_
       clock_gettime (CLOCK_MONOTONIC, &next_time);
       print_console ("After sleep time: ");
       print_console_int (next_time.tv_sec);
       print_console (".");
       print_console_int (next_time.tv_nsec);
       print_console_newline ();
#endif // _ZEPHYR__VERBOSE_
      tests_reports__test_ok();
    }
    counter++;
    TS_INC(next_time, period);

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
  }
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
// #include "../headers/headers.h"

// #define PERIOD 800000000 // -> 0,8 seg
// const struct timespec period = TS (0, PERIOD);


// int main(int argc, char const *argv[])
// {

//   int counter = 0;
//   struct timespec next_time;

//   m2osinit();
//   check_posix_api();
//   console_init(115200);

//   print_console("\nDelay and Clock Performance Test\n");

//   print_console("\nMain\n");

//   measurements_hires__init();
//   measurements_hires__start_measurement();  

//   clock_gettime(CLOCK_MONOTONIC, &next_time);

//   while (1) {
//     print_console("looping...\n");
//     if (counter == 1) {
//       measurements_hires__end_measurement();
//       measurements_hires__finish();
//       tests_reports__test_ok();
//     }
//     counter++;
//     TS_INC(next_time, period);
//     clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
//   }

//   print_console("\nMain ends\n");
//   return 0;
// }
