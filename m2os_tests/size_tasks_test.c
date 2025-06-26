#include "../headers/headers.h"

#define NUM_THREADS 3

THREAD_POOL (NUM_THREADS);

void *
thread (void *arg)
{
  return NULL; //task does nothing
}

int
main (int argc, char const *argv[])
{
  m2osinit ();
  check_posix_api ();
  puts ("Size Tasks Test\n");
  puts ("NUM_THREADS: ");
  putint (NUM_THREADS);
  // threads stuff
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  struct sched_param sch_param;

  // create threads
  for (int i = 0; i < NUM_THREADS; i++)
    {
      pthread_attr_init (&attr);
      sch_param.sched_priority = 3;
      pthread_attr_setschedparam (&attr, &sch_param);
      pthread_create (&threads[i], &attr, thread, NULL);
    }


  return 0;
}
