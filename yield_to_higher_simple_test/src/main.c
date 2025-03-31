#include "../headers/headers.h"

#include <semaphore.h>

THREAD_POOL(2);

#define NUM_THREADS 2
#define STACK_SIZE 512

#define COUNTER_HP_END_VALUE 11
int counter_hp = 0;
int counter_lp = 0;

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

#define PERIOD_LP_NS 200000000
#define PERIOD_HP_NS 100000000
#define DELAY_ACTIVATION_LP_NS (PERIOD_HP_NS / 10)

const struct timespec period_lp = TS(0, PERIOD_LP_NS);
const struct timespec period_hp = TS(0, PERIOD_HP_NS);
const struct timespec eat_lp = TS(0, PERIOD_HP_NS);

#define ARRAY_SIZE 10

void puts_now(char *msg)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  printf("%ld s %09ld ms %s\n", now.tv_sec, now.tv_nsec, msg);
}

void check_stack_usage(k_tid_t thread, struct k_thread *thread_data)
{
  size_t total_stack = k_thread_stack_size_get(thread_data);
  // size_t unused_stack = k_thread_stack_space_get(thread_data);
  size_t *unused_ptr;
  size_t unused_stack = k_thread_stack_space_get(thread, &unused_ptr);
  size_t used_stack = total_stack - unused_stack;

  // get stack pointer

  printf("Stack total: %zu bytes\n", total_stack);
  printf("Stack usado: %zu bytes\n", used_stack);
  printf("Stack libre: %zu bytes\n", unused_stack);
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

struct timespec next_activation_time_ls = TS(0, 0);

void *
task_l()
{
  while (1)
  {
    puts_now("Task LP\n");
    print_thread_stack_base();
    print_stack_pointer();
    print_stack_info();

    tests_reports__assert(counter_hp / 2 == counter_lp * 2 + 1);
    counter_lp++;

    const int value = 10 * counter_hp + counter_lp;
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
    puts_now("Task LP: Thread LP: before yield_to_higher()\n");
    const int stack_before = get_stack_pointer();
    k_yield(); // FIXME -> revisar si es la llamada correcta
    tests_reports__assert(stack_before == get_stack_pointer());
    puts_now("Thread LP: after yield_to_higher()\n");
    tests_reports__assert(counter_hp == counter_lp * 2);

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      printf("a[%d] = %d\n", i, a[i]);
      tests_reports__assert(a[i] == value);
    }

    printf("\n");
    puts_now("Task LP: Thread LP: clock_nanosleep()\n");
    TS_INC(next_activation_time_ls, period_lp);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_ls, NULL);
  }

  return NULL;
}

//********//
// TASK-H //
//********//

struct timespec next_activation_time_hp = TS(0, 0);
void *
task_h()
{
  while (1)
  {
    puts_now("Task HP\n");
    print_thread_stack_base();
    print_stack_pointer();
    print_stack_info();
    counter_hp++;

    tests_reports__assert(counter_hp / 2 == counter_lp);

    if (counter_hp == COUNTER_HP_END_VALUE)
    {
      tests_reports__test_ok();
    }
    puts_now("Task HP: clock_nanosleep()\n");
    TS_INC(next_activation_time_hp, period_hp);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation_time_hp, NULL);
  }

  return NULL;
}

int main(int argc, char const *argv[])
{
  // TODO

  int rc;

  pthread_t thread_h, thread_l;
  pthread_attr_t attr_h, attr_l;
  struct sched_param sch_param_h = {.sched_priority = 20};
  struct sched_param sch_param_l = {.sched_priority = 10};

  m2osinit();
  check_posix_api();
  console_init(115200);

  print_console("\nYield to higher simple test\n");
  print_console("\nMain starts\n");

  print_stack_pointer();
  print_stack_info();

  clock_gettime(CLOCK_MONOTONIC, &next_activation_time_hp);
  const struct timespec delay_activation_hp = TS(0, DELAY_ACTIVATION_LP_NS);
  TS_ADD(next_activation_time_hp, next_activation_time_hp, delay_activation_hp);

  pthread_attr_init(&attr_h);
  pthread_attr_init(&attr_l);

  // Stack size
  pthread_attr_setstacksize(&attr_h, STACK_SIZE);
  pthread_attr_setstacksize(&attr_l, STACK_SIZE);

  // Politica de planificación
  rc = pthread_attr_setschedpolicy(&attr_h, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");
  rc = pthread_attr_setschedpolicy(&attr_l, SCHED_FIFO);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedpolicy");

  // Prioridades
  rc = pthread_attr_setschedparam(&attr_h, &sch_param_h);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");
  rc = pthread_attr_setschedparam(&attr_l, &sch_param_l);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setschedparam");

  // Thread stack
  rc = pthread_attr_setstack(&attr_h, thread_stack[0], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");
  rc = pthread_attr_setstack(&attr_l, thread_stack[1], STACK_SIZE);
  if (rc != 0)
    handle_error_en(rc, "pthread_attr_setstack");

  // main ends
  printf("Main ends\n");
  // check_stack_usage();
  print_stack_info();
  print_thread_stack_base();
  print_stack_info();

  // Create threads
  rc = pthread_create(&thread_h, &attr_h, task_h, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");
  rc = pthread_create(&thread_l, &attr_l, task_l, NULL);
  if (rc != 0)
    handle_error_en(rc, "pthread_create");

  // join thread
  pthread_join(thread_h, NULL);
  pthread_join(thread_l, NULL);
  print_console("\nMain ends\n");
  return 0;
}
