#include "../headers/headers.h"

#include <inttypes.h>

THREAD_POOL(2);

#define NUM_THREADS 5
#define STACK_SIZE 512
#define NUM_OF_LOOPS 100

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

struct timespec eat_times[NUM_THREADS] = {
    TS(0, 3000000),
    TS(0, 7000000),
    TS(0, 12000000),
    TS(0, 16000000),
    TS(0, 37000000)};

struct timespec eat_times_v2[NUM_THREADS] = {
    TS(0, 1800000),
    TS(0, 5800000),
    TS(0, 10800000),
    TS(0, 14800000),
    TS(0, 35800000)};

struct timespec period[NUM_THREADS] = {TS(0, 25000000), TS(0, 40000000), TS(0, 60000000), TS(0, 100000000),
                                       TS(0, 140000000)};
// k_timeout_t period_k[NUM_THREADS] = {K_NSEC(25000000), K_NSEC(40000000), K_NSEC(60000000), K_NSEC(100000000),
//                                      K_NSEC(140000000)};
k_timeout_t period_k[NUM_THREADS] = {K_NSEC(38000000), K_NSEC(53000000), K_NSEC(73000000), K_NSEC(113000000),
                                     K_NSEC(153000000)};

struct timespec deadline[NUM_THREADS] = {TS(0, 25000000), TS(0, 40000000), TS(0, 60000000), TS(0, 100000000),
                                         TS(0, 140000000)};

// more vars ??
struct timespec actual_wakeup[NUM_THREADS];
struct timespec next_activation_time[NUM_THREADS];
hwtime_t response_times[NUM_THREADS] = {0, 0, 0, 0, 0};
int missed_deadlines[NUM_THREADS] = {0, 0, 0, 0, 0};

bool finished = false;

int counter_t0 = 0;
void *task0()
{
  while (!finished)
  {
    // printf("Task 0 ");
    clock_gettime(CLOCK_MONOTONIC, &actual_wakeup[0]);
    tests_reports__eat(eat_times[0]); // eat time
    // printf("-- task 0 done\n");

    // do calculations
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // printf("Actual wakeup: %lld s %ld ns\n", actual_wakeup[0].tv_sec, actual_wakeup[0].tv_nsec);
    // printf("Current time: %lld s %ld ns\n", current_time.tv_sec, current_time.tv_nsec);
    // printf("Current - Actual diff: %lld s %ld ns | eat time: %lld s, %ld ns\n", current_time.tv_sec - actual_wakeup[0].tv_sec, current_time.tv_nsec - actual_wakeup[0].tv_nsec, eat_times[0].tv_sec, eat_times[0].tv_nsec);

    // response time
    hwtime_t response_time = TS2HWT(current_time) - TS2HWT(next_activation_time[0]);
    // store worst case response time
    if (response_time > response_times[0])
    {
      response_times[0] = response_time;
      struct timespec ts;
      HWT2TS(ts, response_time);
      printf("Response time: %lld s %ld ns\n", ts.tv_sec, ts.tv_nsec);
    }

    if (response_time > TS2HWT(deadline[0]))
    {
      missed_deadlines[0]++;
      printf("Missed deadline task 0\n");
      struct timespec ts;
      HWT2TS(ts, response_time);
      printf("Response time: %lld s %ld ns\n", ts.tv_sec, ts.tv_nsec);
    }

    // sleep
    TS_INC(next_activation_time[0], period[0]);
    // printf("Next activation time: %lld s %ld ns\n", next_activation_time[0].tv_sec, next_activation_time[0].tv_nsec);

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time[0], NULL);
    counter_t0++;
  }
  return NULL;
}

int counter_t1 = 0;
void *task1()
{
  while (!finished)
  {
    // printf("Task 1 ");
    clock_gettime(CLOCK_MONOTONIC, &actual_wakeup[1]);
    tests_reports__eat(eat_times[1]); // eat time
    // printf("-- task 1 done\n");

    // do calculations
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // printf("Actual wakeup: %lld s %ld ns\n", actual_wakeup[1].tv_sec, actual_wakeup[1].tv_nsec);
    // printf("Current time: %lld s %ld ns\n", current_time.tv_sec, current_time.tv_nsec);

    // printf("Current - Actual diff: %lld s %ld ns | eat time: %lld s, %ld ns\n", current_time.tv_sec - actual_wakeup[1].tv_sec, current_time.tv_nsec - actual_wakeup[1].tv_nsec, eat_times[1].tv_sec, eat_times[1].tv_nsec);

    // response time
    hwtime_t response_time = TS2HWT(current_time) - TS2HWT(next_activation_time[1]);
    // store worst case response time
    if (response_time > response_times[1])
      response_times[1] = response_time;

    if (response_time > TS2HWT(deadline[1]))
      missed_deadlines[1]++;

    // sleep
    TS_INC(next_activation_time[1], period[1]);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time[1], NULL);
    // // k_sleep(K_TIMEOUT_ABS_NS(period[1].tv_nsec));
    // k_sleep(period_k[1]);
    counter_t1++;
  }

  return NULL;
}
int counter_t2 = 0;
void *task2()
{
  while (!finished)
  {
    clock_gettime(CLOCK_MONOTONIC, &actual_wakeup[2]);
    tests_reports__eat(eat_times[2]); // eat time

    // do calculations
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // response time
    hwtime_t response_time = TS2HWT(current_time) - TS2HWT(next_activation_time[2]);
    // store worst case response time
    if (response_time > response_times[2])
      response_times[2] = response_time;

    if (response_time > TS2HWT(deadline[2]))
      missed_deadlines[2]++;

    // sleep
    TS_INC(next_activation_time[2], period[2]);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time[2], NULL);

    counter_t2++;
  }

  return NULL;
}
int counter_t3 = 0;
void *task3()
{
  while (!finished)
  {
    // printf("Task 3 ");
    clock_gettime(CLOCK_MONOTONIC, &actual_wakeup[3]);
    tests_reports__eat(eat_times[3]); // eat time
    // printf("-- task 3 done\n");

    // do calculations
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // response time
    hwtime_t response_time = TS2HWT(current_time) - TS2HWT(next_activation_time[3]);
    // store worst case response time
    if (response_time > response_times[3])
      response_times[3] = response_time;

    if (response_time > TS2HWT(deadline[3]))
      missed_deadlines[3]++;

    // sleep
    TS_INC(next_activation_time[3], period[3]);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time[3], NULL);

    counter_t3++;
  }

  return NULL;
}

int counter = 0;
void *task4()
{
  while (counter < NUM_OF_LOOPS)
  {
    // printf("Task 4 ");
    clock_gettime(CLOCK_MONOTONIC, &actual_wakeup[4]);
    tests_reports__eat(eat_times[4]); // eat time
    // printf("-- task 4 done\n");

    // do calculations
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // printf("Current - Actual diff: %lld s %ld ns | eat time: %lld s, %ld ns\n", current_time.tv_sec - actual_wakeup[4].tv_sec, current_time.tv_nsec - actual_wakeup[4].tv_nsec, eat_times[4].tv_sec, eat_times[4].tv_nsec);

    // response time
    hwtime_t response_time = TS2HWT(current_time) - TS2HWT(next_activation_time[4]);
    // store worst case response time
    if (response_time > response_times[4])
      response_times[4] = response_time;

    if (response_time > TS2HWT(deadline[4]))
      missed_deadlines[4]++;

    // sleep
    TS_INC(next_activation_time[4], period[4]);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time[4], NULL);

    // // k_sleep(K_TIMEOUT_ABS_NS(period[4].tv_nsec));
    // k_sleep(period_k[4]);
    counter++;
  }
  finished = true;
  // print misse deadlines and worst case response time
  for (int i = 0; i < NUM_THREADS; i++)
  {
    struct timespec ts;
    HWT2TS(ts, response_times[i]);
    printf("Task %d missed deadlines: %d | worst response time %lld s %ld ns\n", i, missed_deadlines[i], ts.tv_sec, ts.tv_nsec);
  }

  printf("Task 0 counter: %d\n", counter_t0);
  printf("Task 1 counter: %d\n", counter_t1);
  printf("Task 2 counter: %d\n", counter_t2);
  printf("Task 3 counter: %d\n", counter_t3);
  printf("Task 4 counter: %d\n", counter);

  return NULL;
}

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t tasks[NUM_THREADS];
  pthread_attr_t attr[NUM_THREADS];
  struct sched_param sch_params[NUM_THREADS] = {
      {.sched_priority = 5},
      {.sched_priority = 4},
      {.sched_priority = 3},
      {.sched_priority = 2},
      {.sched_priority = 1}};

  print_console("\nMulti-task test\n");
  print_console("\nMain starts\n");
  printf("Loops %d\n", NUM_OF_LOOPS);
  // CODE HERE
  printf("MAX prio %d\n", sched_get_priority_max(SCHED_RR));
  printf("MIN prio %d\n", sched_get_priority_min(SCHED_RR));

  printf("Starting tasks\n");

  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_attr_init(&attr[i]);
  }

  // Stack size
  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_attr_setstacksize(&attr[i], STACK_SIZE);
  }

  // Politica de planificación
  for (int i = 0; i < NUM_THREADS; i++)
  {
    rc = pthread_attr_setschedpolicy(&attr[i], SCHED_RR);
    if (rc != 0)
      handle_error_en(rc, "pthread_attr_setschedpolicy");

    rc = pthread_attr_setinheritsched(&attr[i], PTHREAD_EXPLICIT_SCHED);
    if (rc != 0)
      handle_error_en(rc, "pthread_attr_setinheritsched");
  }

  // Prioridades
  for (int i = 0; i < NUM_THREADS; i++)
  {
    rc = pthread_attr_setschedparam(&attr[i], &sch_params[i]);
    if (rc != 0)
      handle_error_en(rc, "pthread_attr_setschedparam");
  }

  // Thread stack
  for (int i = 0; i < NUM_THREADS; i++)
  {
    rc = pthread_attr_setstack(&attr[i], thread_stack[i], STACK_SIZE);
    if (rc != 0)
      handle_error_en(rc, "pthread_attr_setstack");
  }
  printf("Creating tasks\n");

  // print pthread_t
  for (int i = 0; i < NUM_THREADS; i++)
  {
    printf("Task %d: %" PRIu32 "\n", i, tasks[i]);
  }

  clock_gettime (CLOCK_MONOTONIC, &next_activation_time[0]);
  actual_wakeup[0] = next_activation_time[0];
  actual_wakeup[1] = next_activation_time[0];
  actual_wakeup[2] = next_activation_time[0];
  actual_wakeup[3] = next_activation_time[0];
  actual_wakeup[4] = next_activation_time[0];

  next_activation_time[1] = next_activation_time[0];
  next_activation_time[2] = next_activation_time[0];
  next_activation_time[3] = next_activation_time[0];
  next_activation_time[4] = next_activation_time[0];

  // Create threads
  rc = pthread_create(&tasks[0], &attr[0], task0, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&tasks[1], &attr[1], task1, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&tasks[2], &attr[2], task2, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&tasks[3], &attr[3], task3, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&tasks[4], &attr[4], task4, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");

  // join thread
  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(tasks[i], NULL);
  }

  print_console("\nMain ends\n");
  return 0;
}
