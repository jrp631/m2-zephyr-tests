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

struct shared_resource
{
  int th_h_count;
  int th_l_count;
};

struct th_args
{
  char *name;
  struct timespec *c_time;
  struct timespec *c_in_mutex;
  struct timespec *period;
  struct timespec *next_activation;
};

struct shared_resource shared_res = {0, 0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex for shared resource


K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

unsigned int cpu = 0;

#define BUFFER_SIZE 10

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

//********//
// TASK-L //
//********//


void *
task_l(void *args)
{
  int local_count = 0;
  struct th_args *th_args = (struct th_args *)args;

  while (local_count < MAX_LOOPS)
  {
    // TODO
    local_count++;
    //eat
    printf("Thread %s llega y hace eat()\n", th_args->name);
    eat_ts(th_args->c_time);
    //mutex
    pthread_mutex_lock(&mutex);
    shared_res.th_l_count++;
    printf("Thread en seccion critica %s count: %d\n", th_args->name, shared_res.th_l_count);
    eat_ts(th_args->c_in_mutex);
    pthread_mutex_unlock(&mutex);
    // sleep
    incr_timespec(th_args->next_activation, th_args->period);
    printf("Thread %s siguiente activacion %f\n", th_args->name,
           timespec_to_double(th_args->next_activation));
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
        th_args->next_activation, NULL);
  }
  printk("Thread %s termina\n", "task_l");
  pthread_exit(NULL);
}

//********//
// TASK-H //
//********//

void *
task_h(void *args)
{
  int local_count = 0;
  struct th_args *th_args = (struct th_args *)args;
  // TODO: rewrite
  while (local_count < MAX_LOOPS)
  {
    // TODO
    local_count++;
    //eat 
    //mutex
    printf("Thread %s llega y hace eat()\n", th_args->name);
    eat_ts(th_args->c_time);
    pthread_mutex_lock(&mutex);
    shared_res.th_h_count++;
    printf("Thread en seccion critica %s count: %d\n", th_args->name, shared_res.th_h_count);
    eat_ts(th_args->c_in_mutex);
    pthread_mutex_unlock(&mutex);
    // sleep 
    incr_timespec(th_args->next_activation, th_args->period);
    printk("Thread %s siguiente activacion %f\n", th_args->name,
           timespec_to_double(th_args->next_activation));
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
        th_args->next_activation, NULL);
  }
  printf("Thread %s termina\n", "task_h");
  pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
  printf("Two tasks access to a shared resource");
  pthread_t th1, th2;
  pthread_attr_t attr_th1, attr_th2;
  struct sched_param sch_param_th1, sch_param_th2;
  struct timespec c_time_th1,c_time_th1_mutex,c_time_th2, c_time_th2_mutex; // TODO: determinar valores
  struct timespec period_th1, period_th2; // TODO: determinar valores

  struct timespec next_activation;
  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  // TODO full args
  //C_time th1 = 0,8 sec | C_time_mutex th1 = 0,3 sec | period th1 = 2 sec
  c_time_th1.tv_sec = 0;
  c_time_th1.tv_nsec = 800000000;
  c_time_th1_mutex.tv_sec = 0;
  c_time_th1_mutex.tv_nsec = 300000000;
  period_th1.tv_sec = 2;
  period_th1.tv_nsec = 0;
  struct th_args th1_args = { "th1", &c_time_th1, &c_time_th1_mutex, &period_th1, &next_activation};


  // C_time th2 = 0,8 sec | C_time_mutex th2 = 0,2 sec | period th2 = 3 sec
  c_time_th2.tv_sec = 0;
  c_time_th2.tv_nsec = 800000000;
  c_time_th2_mutex.tv_sec = 0;
  c_time_th2_mutex.tv_nsec = 200000000;
  period_th2.tv_sec = 3;
  period_th2.tv_nsec = 0;
  struct th_args th2_args = { "th2", &c_time_th2, &c_time_th2_mutex, &period_th2, &next_activation};


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

  CHK(pthread_create(&th1, &attr_th1, task_l, &th1_args));
  CHK(pthread_create(&th2, &attr_th2, task_h, &th2_args));

  CHK(pthread_join(th1, NULL));
  CHK(pthread_join(th2, NULL));

  printf("Thread %s count: %d\n", th1_args.name, shared_res.th_l_count);
  printf("Thread %s count: %d\n", th2_args.name, shared_res.th_h_count);
  printf("Thread %s termina\n", "main");

  return 0;
}
