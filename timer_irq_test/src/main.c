#include "../headers/headers.h"

#define NUM_OF_LOOPS 20000
uint64_t avg_time_ns;
float avg_time;

int num_of_longer_times = 0;
uint64_t sum_of_longer_times = 0;

int main(int argc, char **argv)
{
  m2osinit();
  console_init(115200);
  printf("\nTimer_IRQ_Test ");
  printf("%d\n", NUM_OF_LOOPS);
  printf("Main starts\n");

  int iter = 0;
  while (iter < 10)
  {
    // measure time to make measurements
    measurements_hires__init();

    for (int i = 0; i < NUM_OF_LOOPS; i++)
    {
      measurements_hires__start_measurement();
      measurements_hires__end_measurement();
    }
    avg_time_ns = measurements_hires__measurement_avg_ns();
    avg_time = (float)measurements_hires__measurement_time() / (float)NUM_OF_LOOPS;
    printf("Avg time:");
    printf("%" PRIu64, avg_time_ns);
    printf("ns ");
    printf("Time Units  ");
    measurements_hires__print_measures_data();

    // Detect times longer than expected
    num_of_longer_times = 0;
    sum_of_longer_times = 0;
    for (int i = 0; i < NUM_OF_LOOPS / 30; i++)
    {
      measurements_hires__init();
      measurements_hires__start_measurement();
      measurements_hires__end_measurement();
      uint64_t measurement_time = measurements_hires__measurement_time();
      if (measurement_time > (avg_time * 1.5))
      {
        printf(" T:");
        printf("%" PRIu64, measurement_time);
        num_of_longer_times++;
        sum_of_longer_times += measurements_hires__measurement_time();
      }
    }

    printf("\n");
    printf("Measurement Timer handler time:");
    //FIXME print algo
    uint64_t dato = (measurements_hires__to_time_units(sum_of_longer_times) /((uint64_t)num_of_longer_times - avg_time_ns));
    printf("%" PRIu64, dato);
    printf("ns");
    printf(" (Num_Of_Longer_Times:");
    printf("%d", num_of_longer_times);
    printf(" Sum_Of_Longer_Times:");
    printf("%" PRIu64, sum_of_longer_times);
    printf(")\n");

    measurements_hires__finish();
    iter++;
  }
  printf("\nMain ends");
  return 0;
}
