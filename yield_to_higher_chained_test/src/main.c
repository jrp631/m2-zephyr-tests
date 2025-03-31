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

#define PERIOD_LP_NS 480000000
#define PERIOD_MP_NS 200000000
#define PERIOD_HP_NS 100000000
#define DELAY_ACTIVATION_LP_NS (PERIOD_HP_NS / 10)
const struct timespec period_lp = TS(0, PERIOD_LP_NS);
const struct timespec period_mp = TS(0, PERIOD_MP_NS);
const struct timespec period_hp = TS(0, PERIOD_HP_NS);
const struct timespec eat_lp = TS(0, PERIOD_MP_NS);
const struct timespec eat_mp = TS(0, PERIOD_HP_NS);

#define ARRAY_SIZE 10

void puts_now(char *msg)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  printf("%ld s %09ld ms %s\n", now.tv_sec, now.tv_nsec, msg);
}

void print_stack_info(void)
{
  struct k_thread *current_thread = k_current_get();
  void *stack_base = current_thread->stack_info.start;
  size_t stack_size = current_thread->stack_info.size;

  printf("Thread: %p\n", current_thread);
  printf("Stack base: %p\n", stack_base);
  printf("Stack size: %zu bytes\n", stack_size);
}

void print_thread_stack_base(void)
{
  printk("Stack base: %p\n", thread_stack);
}

uint32_t get_stack_pointer(void)
{
  uint32_t sp;
  __asm volatile("mov %0, sp" : "=r"(sp));
  return sp;
}

void print_stack_pointer(void)
{
  uint32_t sp = get_stack_pointer();
  printk("Stack Pointer actual: %p\n", (void *)sp);
}

//********//
// TASK-L //
//********//

struct timespec next_activation_time_lp = TS(0, 0);

void *
task_l()
{

  while (1)
  {
    puts_now("Task LP\n");
    print_thread_stack_base();
    print_stack_pointer();
    print_stack_info();

    count_lp++;
    if (count_lp == COUNT_LP_END_VALUE)
    {
      tests_reports__assert(count_hp == 3 && count_mp == 3);
      tests_reports__test_ok();
    }
    const int value = 100 * count_hp + 10 * count_mp + count_lp;
    int a[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      a[i] = value;
    }
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      printf("a[%d] = %d\n", i, a[i]);
      tests_reports__assert(a[i] == value);
    }
    printf("\n");

    puts_now("Task LP: Thread LP: eat\n");
    tests_reports__eat(eat_lp);
    tests_reports__assert(count_hp == 1 && count_mp == 1 && count_lp == 1);

    puts_now("Task LP: Thread LP: k_yield()\n");
    // const int stack_before = m2_hal_regs_get_sp_reg();
    const uint32_t stack_before = get_stack_pointer();
    k_yield();
    tests_reports__assert(stack_before == get_stack_pointer());
    puts_now("Task LP: Thread LP: after k_yield()\n");

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      printf("a[%d] = %d\n", i, a[i]);
      tests_reports__assert(a[i] == value);
    }
    printf("\n");
    test_reports__assert(count_hp == 2 && count_mp == 2 && count_lp == 1);
    printf("Task LP: Thread LP: clock_nanosleep()\n");
    TS_INC(next_activation_time_lp, period_lp);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_lp, NULL);
  }
  return NULL;
}

//********//
// TASK-m //
//********//

struct timespec next_activation_time_mp = TS(0, 0);
int first_activation_mp = 1;

void *
task_m()
{
  while (1)
  {
    puts_now("Task MP\N");
    print_thread_stack_base();
    print_stack_pointer();
    print_stack_info();

    count_mp++;
    if (first_activation_mp)
    {
      first_activation_mp = 0;
      tests_reports__assert(count_hp == 1 && count_lp == 0);
      puts_now("Task MP: first clock_nanosleep()\n");
      TS_INC(next_activation_time_mp, period_mp);
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_mp, NULL);
    }

    const int value = 100 * count_hp + 10 * count_mp + count_lp;
    int a[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      a[i] = value;
    }
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      printf("a[%d] = %d\n", i, a[i]);
      tests_reports__assert(a[i] == value);
    }
    printf("\n");

    if (count_mp == 2)
    {
      puts_now("Thread MP: eat\n");
      tests_reports__eat(eat_mp);
    }
    tests_reports__assert((count_mp == 2 && count_hp == 1 && count_lp == 1) ||
                          (count_mp == 3 && count_hp == 3 && count_lp == 1));
    puts_now("Thread MP: before k_yield()\n");
    const int stack_before = get_stack_pointer();
    k_yield();
    tests_reports__assert(stack_before == get_stack_pointer());
    puts_now("Thread MP: after yield_to_higher()\n");

    tests_reports__assert((count_mp == 2 && count_hp == 2 && count_lp == 1) ||
                          (count_mp == 3 && count_hp == 3 && count_lp == 1));

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      putint(a[i]);
      puts(" ");
      tests_reports__assert(a[i] == value);
    }
    puts("\n");

    puts_now("Thread MP: clock_nanosleep()\n");
    TS_INC(next_activation_time_mp, period_mp); // XXX lp
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
                    &next_activation_time_mp, NULL); // XXX lp
  }
}

//********//
// TASK-H //
//********//

struct timespec next_activation_time_hp = TS(0, 0);
int first_activation_hp = 1;

void *
task_h()
{
  while (1)
  {
    puts_now("Task HP\n");
    print_thread_stack_base();
    print_stack_pointer();
    print_stack_info();

    count_hp++;
    if (first_activation_hp)
    {
      first_activation_hp = 0;
      tests_reports__assert(count_hp == 1 && count_lp == 0 && count_mp == 0);
      puts_now("Task HP: first clock_nanosleep()\n");
      TS_INC(next_activation_time_hp, period_hp);
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_hp, NULL);
    }
    tests_reports__assert(count_mp == 2 && count_lp == 1);
    puts_now("Task HP: clock_nanosleep()\n");
    TS_INC(next_activation_time_hp, period_hp);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_hp, NULL);
  }

  return NULL;
}

int main(int argc, char const *argv[])
{
  int rc;

  pthread_t thread_h, thread_l, thread_m;
  pthread_attr_t attr_h, attr_l, attr_m;
  struct sched_param sch_param_h = {.sched_priority = 30};
  struct sched_param sch_param_m = {.sched_priority = 20};
  struct sched_param sch_param_l = {.sched_priority = 10};

  m2osinit();
  check_posix_api();
  console_init(115200);

  // init measurements

  print_console("\Yield to higher chained test\n");
  print_console("\nMain starts\n");
  // CODE HERE
  print_stack_pointer();
  print_stack_info();

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
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setschedparam(&attr_m, &sch_param_m);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setschedparam(&attr_l, &sch_param_l);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");

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
  print_stack_pointer();
  print_stack_info();

  // join thread
  pthread_join(thread_h, NULL);
  pthread_join(thread_m, NULL);
  pthread_join(thread_l, NULL);
  print_console("\nMain ends\n");
  return 0;
}
