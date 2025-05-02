// Simple test of yield_to_higher() using the M2OS POSIX-like layer.
// The high priority thread (thread_body_hp) is always ready when thread_body_lp
// calls yield_to_higher().
//
// th_hp #......._#......#......._#......#..   ���       ^ = thread activation
//       ^       ^       ^       ^       ^               # = thread execution
// th_lp .########.#......########.#......##   ���       _ = thread active
//        ^       y       ^      y        ^              . = thread suspended
//       0       1       2       3       4               y = yield_to_higher
//
#include "../headers/headers.h"
#include <zephyr/debug/thread_analyzer.h>

THREAD_POOL(2);

#define NUM_THREADS 2
#define STACK_SIZE 512
#define MAX_LOOPS 3

#define PERIOD_LP_NS 200000000
#define PERIOD_HP_NS 100000000
#define DELAY_ACTIVATION_LP_NS (PERIOD_HP_NS / 10)
const struct timespec period_lp = TS (0, PERIOD_LP_NS);
const struct timespec period_hp = TS (0, PERIOD_HP_NS);
const struct timespec eat_lp = TS (0, PERIOD_HP_NS);

#define COUNTER_HP_END_VALUE 11

struct th_args
{
  char *name;
  struct timespec *c_time;
  struct timespec *period;
  struct timespec *next_activation;
};

void puts_now(char *msg)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  printf("%lld", now.tv_sec);
  printf("s");
  printf("%ld", now.tv_nsec / 100000);
  printf("ms ");
  puts(msg);
}

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

unsigned int cpu = 0;

//********//
// TASK-L //
//********//

int counter_lp = 0;

struct timespec next_activation_time_lp = TS (0, 0);

void *
task_l(void *args)
{
  while (counter_lp < COUNTER_HP_END_VALUE)
  {
    puts_now("Thread LP - before eat");
    counter_lp++;
    eat_ts(&eat_lp);
    puts_now("Thread LP - after eat");
    TS_INC(next_activation_time_lp, period_lp);
    printf("Thread LP - next activation: %lld.%ld\n", next_activation_time_lp.tv_sec, next_activation_time_lp.tv_nsec);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &next_activation_time_lp, NULL);
  }
}

//********//
// TASK-H //
//********//

int counter_hp = 0;

struct timespec next_activation_time_hp = TS (0, 0);


void *
task_h(void *args)
{
  while (counter_hp < COUNTER_HP_END_VALUE)
  {
    puts_now("Thread HP");
    counter_hp++;
    TS_INC(next_activation_time_hp, period_hp);
    printf("Thread HP - next activation: %lld.%ld\n", next_activation_time_hp.tv_sec, next_activation_time_hp.tv_nsec);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &next_activation_time_hp, NULL);
  }
  
  return NULL;
}

int main(int argc, char const *argv[])
{

  printf("Yield to higher test ported to Zehpyr w/o explicit preemption points\n");
  pthread_t th1, th2;
  pthread_attr_t attr_th1, attr_th2;
  struct sched_param sch_param_th1, sch_param_th2;

  // TODO
  CHK(pthread_attr_init(&attr_th1));
  CHK(pthread_attr_init(&attr_th2));

  CHK(pthread_attr_setstack(&attr_th1, thread_stack[0], STACK_SIZE));
  CHK(pthread_attr_setstack(&attr_th2, thread_stack[1], STACK_SIZE));

  CHK(pthread_attr_setinheritsched(&attr_th1, PTHREAD_EXPLICIT_SCHED));
  CHK(pthread_attr_setschedpolicy(&attr_th1, SCHED_RR));
  CHK(pthread_attr_setinheritsched(&attr_th2, PTHREAD_EXPLICIT_SCHED));
  CHK(pthread_attr_setschedpolicy(&attr_th2, SCHED_RR));
  sch_param_th1.sched_priority = 14;
  sch_param_th2.sched_priority = 10;

  CHK(pthread_attr_setschedparam(&attr_th1, &sch_param_th1));
  CHK(pthread_attr_setschedparam(&attr_th2, &sch_param_th2));

  CHK(pthread_attr_setdetachstate(&attr_th1, PTHREAD_CREATE_JOINABLE));
  CHK(pthread_attr_setdetachstate(&attr_th2, PTHREAD_CREATE_JOINABLE));

  CHK(pthread_create(&th1, &attr_th1, task_h, NULL));
  CHK(pthread_create(&th2, &attr_th2, task_l, NULL));

  CHK(pthread_join(th1, NULL));
  CHK(pthread_join(th2, NULL));

  printf("Thread HP counter: %d\n", counter_hp);
  printf("Thread LP counter: %d\n", counter_lp);

  printf("Thread HP finished\n");

  //print stack info???

  return 0;
}
