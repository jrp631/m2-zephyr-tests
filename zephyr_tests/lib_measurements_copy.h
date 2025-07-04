#ifndef ZEPHYR_INCLUDE_POSIX_MEASUREMENTS_H_
#define ZEPHYR_INCLUDE_POSIX_MEASUREMENTS_H_

/***
 * Description: This file contains the implementation of the measurements_hires API for ZephyrOS,
 * this allows to make M2OS performance tests portable and easy to manage.
 *
 * Author: Juan Romon Peña
 */
#include <stdbool.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/posix/time.h>
#include <zephyr/posix/timespec_operations.h>
#include <stdio.h>
#include <inttypes.h>


/**
 * MEASUREMENT DATA
 */
bool initialized = false;
long num_of_measurements = 0;

// variables alternativas que usan uint64_t
uint64_t measure_time_offset = 0;
uint64_t total_measurements_time = 0;

uint64_t measurement_start_time = 0;
uint64_t measurement_end_time = 0;

uint64_t max_measurement_time = 0; 

k_tid_t thread;                 // current thread
k_thread_runtime_stats_t stats; // thread stats

/**
 * @brief  Get the cycles of execution of the current thread. 
 * @note   Runtime stats need to be enabled
 * @retval Execution cycles of the current thread, - 1 if runtime stats are not enabled.
 */
static inline uint64_t get_exec_cycles()
{
	
	#ifdef CONFIG_THREAD_RUNTIME_STATS && CONFIG_SCHED_THREAD_USAGE
  k_thread_runtime_stats_get(thread, &stats);
	uint64_t cycles = stats.execution_cycles;
	return cycles;
  #endif // DEBUG
	return -1;
}

static inline uint64_t measurements_hires__time_unit_in_ns()
{
	return (uint64_t)(1000/168);
}

static inline uint64_t measurements_hires__to_time_units (uint64_t time)
{
	return k_cyc_to_ns_floor64(time);
}

// reset
static inline void measurements_hires__reset()
{
	num_of_measurements = 0;

	total_measurements_time = 0;
}

// start
static inline void measurements_hires__start_measurement()
{
	measurement_start_time = sys_clock_cycle_get_64();
}

// end
static inline void measurements_hires__end_measurement()
{
	uint64_t current_time_hw = sys_clock_cycle_get_64();
	total_measurements_time =
		total_measurements_time + (current_time_hw - measurement_start_time);

	num_of_measurements++;

  if (current_time_hw - measurement_start_time > max_measurement_time)
  {
    max_measurement_time = current_time_hw - measurement_start_time;
  }
}

// avg
static inline uint64_t measurements_hires__measurement_avg()
{
	return (uint64_t)((total_measurements_time -
			   (measure_time_offset * num_of_measurements)) /
			  (uint64_t)num_of_measurements);
}

static inline uint64_t measurements_hires__measurement_avg_ns()
{
	return measurements_hires__measurement_avg() * 1000 / 168;
}

/**
 * @brief  Get the measurement time
 * @note   
 * @retval Measurement time
 */
static inline uint64_t measurements_hires__measurement_time()
{
	return total_measurements_time;
}

/**
 * @brief Initialize the measurements_hires module.
 * @note This function initializes the measurements_hires module and calculates the measure time offset     
 * @retval None
 */
static inline void measurements_hires__init() 
{
	int num_of_measures = 1000;
	initialized = true;

	// Get measure_time offset
	for (int i = 0; i < num_of_measures; i++) {
		measurements_hires__start_measurement();
		measurements_hires__end_measurement();
	}

	measure_time_offset = (total_measurements_time / (uint64_t)num_of_measures);

	// Reset values to be ready for the "real" measurements
	num_of_measurements = 0;

	total_measurements_time = 0;
}

/**
 * @brief Print the measurements data.  
 * @note   
 * @retval None
 */
static inline void measurements_hires__print_measures_data()
{
	printf("Total:");
	printf("%"PRIu64, total_measurements_time);
	printf(" N:");
	printf("%ld", num_of_measurements);
	printf(" Off:");
	printf("%"PRIu64, measure_time_offset);
	printf(" Avg:");
	printf("%"PRIu64, measurements_hires__measurement_avg());
	printf(" = ");
	printf("%"PRIu64, k_cyc_to_ns_floor64(measurements_hires__measurement_avg()));
	printf("ns ");
  printf("Max ticks:");
  printf("%"PRIu64, max_measurement_time);
	printf("\n");

}

static inline void measurements_hires__finish()
{
	measurements_hires__print_measures_data();
}

#endif /* ZEPHYR_INCLUDE_POSIX_MEASUREMENTS_H_ */