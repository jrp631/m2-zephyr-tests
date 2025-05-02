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
#define STACK_SIZE 4096

K_THREAD_STACK_ARRAY_DEFINE(thread_stack, NUM_THREADS, STACK_SIZE);

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
  printf(msg);
}

//******//
// TASK //
//******//

void *
task(void *args)
{
  struct th_args *th_args = (struct th_args *)args;
  struct timespec ts;
  double eat_time, tmp;
  struct timespec next_activation = *th_args->next_activation;

  // CHKE (clock_gettime (CLOCK_MONOTONIC, &next_activation));
  // double tmp = timespec_to_double (&next_activation);
  int count = 0;
  while (count < 3)
    {
      // escribe nombre y hora (usa clock_gettime() y timespec_to_double)
      CHKE (clock_gettime (CLOCK_MONOTONIC, &ts));
      tmp = timespec_to_double (&ts);
      printf ("Thread %s hora: %f\n", th_args->name, tmp);
      // ejecuta durante un tiempo (eat)
      eat_time = timespec_to_double(th_args->c_time);
      eat ((float)eat_time);
      // escribe nombre y hora (usa clock_gettime() y timespec_to_double())
      CHKE (clock_gettime (CLOCK_MONOTONIC, &ts));
      tmp = timespec_to_double (&ts);
      printf ("Thread %s despues de eat %f\n", th_args->name, tmp);
      // calcula el siguiente instante de activación (usa incr_timespec())
      incr_timespec (&next_activation, th_args->period);
      printf ("Thread %s siguiente activacion %f\n", th_args->name,
              timespec_to_double (&next_activation));
      // comprueba si supera el plazo  (usa smaller_timespec())
      if (smaller_timespec (&ts, &next_activation))
        {
          printf ("Thread %s dentro el plazo\n", th_args->name);
        }
      else
        {
          printf ("Thread %s sobrepasa superado el plazo\n", th_args->name);
        }
      count++;
      // espera a la siguiente activación (usa clock_nanosleep())
      CHK (clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME,
        &next_activation, NULL));
    }
  printf ("Thread %s termina\n", th_args->name);
    pthread_exit(NULL);

}

int main(int argc, char const *argv[])
{
  printf("Preemtive tasks test with fix priorities\n");
  pthread_t th1, th2, th3;
  pthread_attr_t attr_th1, attr_th2, attr_th3;
  struct sched_param sch_param_th1, sch_param_th2, sch_param_th3;
  struct timespec c_time_th1, c_time_th2, c_time_th3;
  struct timespec period_th1, period_th2, period_th3;
  struct timespec next_activation;

  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  c_time_th1.tv_sec = 1;
  c_time_th1.tv_nsec = 0;
  period_th1.tv_sec = 3;
  period_th1.tv_nsec = 0;
  struct th_args th1_args = { "th1", &c_time_th1, &period_th1, &next_activation};

  c_time_th2.tv_sec = 1;
  c_time_th2.tv_nsec = 0;
  period_th2.tv_sec = 4;
  period_th2.tv_nsec = 0;
  struct th_args th2_args = { "th2", &c_time_th2, &period_th2, &next_activation };

  c_time_th3.tv_sec = 1;
  c_time_th3.tv_nsec = 200000000;
  period_th3.tv_sec = 5;
  period_th3.tv_nsec = 0;
  struct th_args th3_args = { "th3", &c_time_th3, &period_th3, &next_activation };  

  CHK(pthread_attr_init(&attr_th1));
  CHK(pthread_attr_init(&attr_th2));
  CHK(pthread_attr_init(&attr_th3));

  // CHK(pthread_attr_setstacksize(&attr_th1, STACK_SIZE));
  // CHK(pthread_attr_setstacksize(&attr_th2, STACK_SIZE));
  // CHK(pthread_attr_setstacksize(&attr_th3, STACK_SIZE));

  CHK(pthread_attr_setstack(&attr_th1, thread_stack[0], STACK_SIZE));
  CHK(pthread_attr_setstack(&attr_th2, thread_stack[1], STACK_SIZE));
  CHK(pthread_attr_setstack(&attr_th3, thread_stack[2], STACK_SIZE));
  
  CHK(pthread_attr_setinheritsched(&attr_th1, PTHREAD_EXPLICIT_SCHED));
  CHK(pthread_attr_setschedpolicy(&attr_th1, SCHED_RR));

  CHK(pthread_attr_setinheritsched(&attr_th2, PTHREAD_EXPLICIT_SCHED));
  CHK(pthread_attr_setschedpolicy(&attr_th2, SCHED_RR));

  CHK(pthread_attr_setinheritsched(&attr_th3, PTHREAD_EXPLICIT_SCHED));
  CHK(pthread_attr_setschedpolicy(&attr_th3, SCHED_RR));

  sch_param_th1.sched_priority = 14;
  sch_param_th2.sched_priority = 10;
  sch_param_th3.sched_priority = 5;

  CHK(pthread_attr_setschedparam(&attr_th1, &sch_param_th1));
  CHK(pthread_attr_setschedparam(&attr_th2, &sch_param_th2));
  CHK(pthread_attr_setschedparam(&attr_th3, &sch_param_th3));

  CHK(pthread_attr_setdetachstate(&attr_th1, PTHREAD_CREATE_JOINABLE));
  CHK(pthread_attr_setdetachstate(&attr_th2, PTHREAD_CREATE_JOINABLE));
  CHK(pthread_attr_setdetachstate(&attr_th3, PTHREAD_CREATE_JOINABLE));

  printf("Main -> creando threads\n");

  CHK(pthread_create(&th1, &attr_th1, task, &th1_args));
  CHK(pthread_create(&th2, &attr_th2, task, &th2_args));
  CHK(pthread_create(&th3, &attr_th3, task, &th3_args));
  // printf("Main -> threads creados\n");

  // wait for threads to finish
  CHK(pthread_join(th1, NULL));
  CHK(pthread_join(th2, NULL));
  CHK(pthread_join(th3, NULL));

  return 0;
}
