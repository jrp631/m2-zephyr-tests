// Test of chained yield_to_higher() using the M2OS POSIX-like layer.
// Two yield_to_higher() nested.
//
// th_hp #.......................__#.....#....        ^ = thread activation
//       ^                       ^       ^            # = thread execution
// th_mp _#..............___#######_#...._##..        _ = thread active
//       ^               ^        y      ^y           . = thread suspended
// th_lp __################__________#.......#        y = yield_to_higher
//       ^                y                  ^
//       0       1       2       3       4
//
#include "../headers/headers.h"

THREAD_POOL(3);

#define NUM_THREADS 3
#define STACK_SIZE 512

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

#define COUNT_LP_END_VALUE 2
int count_lp = 0;
int count_mp = 0;
int count_hp = 0;

int global_stack_base;

#define PERIOD_LP_NS 400000000
#define PERIOD_MP_NS 200000000
#define PERIOD_HP_NS 400000000
#define DELAY_ACTIVATION_LP_NS (PERIOD_HP_NS / 10)
const struct timespec period_lp = TS(0, PERIOD_LP_NS);
const struct timespec period_mp = TS(0, PERIOD_MP_NS);
const struct timespec period_hp = TS(0, PERIOD_HP_NS);

#define EAT_LP_NS 250000000
#define EAT_MP_NS 400000000
#define EAT_HP_NS 100000000
const struct timespec eat_lp = TS(0, EAT_LP_NS);
const struct timespec eat_mp = TS(0, EAT_MP_NS);
const struct timespec eat_hp = TS(0, EAT_HP_NS);

void puts_now(char *msg)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  printf("%lld", now.tv_sec);
  printf("s");
  printf("%ld", now.tv_nsec / 100000);
  printf("ms ");
  printf(msg);
}

//********//
// TASK-L //
//********//

struct timespec next_activation_time_lp = TS(0, 0);

void *
task_l()
{
  count_lp++; 
  // eats until task m is ready 
  tests_reports__eat(eat_lp); 
  printf("Task LP: after k_yield()\n");
  // yield -> to task m
  k_yield();
  count_lp++; 
  // after returning from yield, terminates
  printf("Task LP: after k_yield()\n");
  pthread_exit(NULL);
}

//********//
// TASK-m //
//********//

struct timespec next_activation_time_mp = TS(0, 0);

void *
task_m()
{

  puts_now("Task MP\n");

  count_mp++;

  puts_now("Task MP: first clock_nanosleep()\n");
  TS_INC(next_activation_time_mp, period_mp);
  printf("Next activation time mp: %lld s %09ld ns\n", next_activation_time_mp.tv_sec, next_activation_time_mp.tv_nsec);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_mp, NULL);
  count_mp++;
  // after wakeup eat for some time
  puts_now("Task MP\n");
  tests_reports__eat(eat_mp); // FIXME 
  // then yield to higher
  puts_now("Task MP: before k_yield()\n");
  k_yield();
  count_mp++;
  puts_now("Task MP: after k_yield()\n");
  //after do something else and terminate the task

  pthread_exit(NULL);
}

//********//
// TASK-H //
//********//

struct timespec next_activation_time_hp = TS(0, PERIOD_MP_NS);

void *
task_h()
{
  puts_now("Task HP\n");
  count_hp++;

  puts_now("Task HP: first clock_nanosleep()\n");
  TS_INC(next_activation_time_hp, period_hp);
  printf("Next activation time hp: %lld s %09ld ns\n", next_activation_time_hp.tv_sec, next_activation_time_hp.tv_nsec);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_hp, NULL); // jumps to task_m
  count_hp++;
  puts_now("Task HP\n");
  // wake up from nanosleep and eat for some time then end
  tests_reports__eat(eat_hp); // TODO: check

  pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t thread_h, thread_l, thread_m;
  pthread_attr_t attr_h, attr_l, attr_m;
  struct sched_param sch_param_h = {.sched_priority = 15};
  struct sched_param sch_param_m = {.sched_priority = 10};
  struct sched_param sch_param_l = {.sched_priority = 5};

  m2osinit();
  check_posix_api();
  console_init(115200);

  // init measurements

  print_console("\nYield to higher chained test\n");
  print_console("\nMain starts\n");
  // CODE HERE

  pthread_attr_init(&attr_h);
  pthread_attr_init(&attr_m);
  pthread_attr_init(&attr_l);

  // Stack size
  pthread_attr_setstacksize(&attr_h, STACK_SIZE);
  pthread_attr_setstacksize(&attr_m, STACK_SIZE);
  pthread_attr_setstacksize(&attr_l, STACK_SIZE);

  // Politica de planificación
  rc = pthread_attr_setschedpolicy(&attr_h, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  rc = pthread_attr_setschedpolicy(&attr_m, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  rc = pthread_attr_setschedpolicy(&attr_l, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");

  // Prioridades
  rc = pthread_attr_setschedparam(&attr_h, &sch_param_h);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam h");
  rc = pthread_attr_setschedparam(&attr_m, &sch_param_m);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam m");
  rc = pthread_attr_setschedparam(&attr_l, &sch_param_l);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam l");

  // Thread stack
  rc = pthread_attr_setstack(&attr_h, thread_stack[0], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  rc = pthread_attr_setstack(&attr_m, thread_stack[1], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  rc = pthread_attr_setstack(&attr_l, thread_stack[2], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  // Create threads
  rc = pthread_create(&thread_h, &attr_h, task_h, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&thread_m, &attr_m, task_m, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&thread_l, &attr_l, task_l, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");

  // main ends
  printf("Main ends\n");
  // print_thread_stack_base();
  // print_stack_pointer();
  // print_stack_info();

  // join thread
  pthread_join(thread_h, NULL);
  pthread_join(thread_m, NULL);
  pthread_join(thread_l, NULL);
  print_console("\nMain ends\n");

  // print counters
  printf("count_hp: %d | count_mp: %d | count_lp: %d\n", count_hp, count_mp, count_lp);
  return 0;
}
