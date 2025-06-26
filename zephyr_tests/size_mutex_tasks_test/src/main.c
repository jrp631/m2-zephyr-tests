#include "../headers/headers.h"

#define NUM_THREADS 9
#define STACK_SIZE 4096 // 4KB same stack as M2OS

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

k_timeout_t period1_k = K_MSEC(100); // 0.1s

#define PRINT_FIELD_INFO(type, field) \
    printf("Field " #field ": offset = %zu, size = %zu\n", offsetof(type, field), sizeof(((type *)0)->field))


//mutex to protect shared resource
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int shared_resource = 0;


void *task();

int main(int argc, char const *argv[])
{

  // Thread vars
  struct k_mutex dummy_mtx;
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr[NUM_THREADS];
  struct sched_param sch_param = {.sched_priority = 3};

  puts("\nSize Tasks Test\n");
  puts("\nMain starts\n");
  puts("\nNum of threads:");
  print_console_int(NUM_THREADS);
  puts("\n");

#ifdef _ZEPHYR__VERBOSE_
  struct k_mutex dummy;
  printf("Size of K_mutext : %d\n", sizeof(dummy_mtx));
  printf("Size of pthread_mutex_t: %d\n", sizeof(mutex));
  PRINT_FIELD_INFO(struct k_mutex, lock_count);
  PRINT_FIELD_INFO(struct k_mutex, owner);
  PRINT_FIELD_INFO(struct k_mutex, wait_q);
  PRINT_FIELD_INFO(struct k_mutex, owner_orig_prio);
#endif // _ZEPHYR__VERBOSE_
  // init attr and create threads

  for (int i = 0; i < NUM_THREADS; i++)
  {
    // attr init
    pthread_attr_init(&attr[i]);
    // stack size
    pthread_attr_setstacksize(&attr[i], STACK_SIZE);
    // scheduling policy
    pthread_attr_setschedpolicy(&attr[i], SCHED_FIFO);
    // priorities
    pthread_attr_setschedparam(&attr[i], &sch_param);
    // thread stack
    pthread_attr_setstack(&attr[i], thread_stack[i], STACK_SIZE);
    // create threads
    pthread_create(&threads[i], &attr[i], task, NULL);
  }

  // join thread
  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }
  pthread_mutex_destroy(&mutex);
  puts("\nMain ends\n");
  return 0;
}

void *
task()
{
  //infinite loop - sleep X amount of time
  while (1)
  {
    pthread_mutex_lock(&mutex);
    shared_resource++;
    pthread_mutex_unlock(&mutex);
    k_sleep(period1_k);
  }
  pthread_exit(NULL);
}
