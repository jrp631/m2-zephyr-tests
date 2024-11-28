#ifndef _M2_TESTS_REPORTS_H_
#define _M2_TESTS_REPORTS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/posix/time.h>
#include <zephyr/posix/load.h>

static inline void 
tests_reports__test_ok () {
  puts ("\nTEST OK\n");
  exit (0);
}

static inline void
tests_reports__assert_equal_c (int32_t val1, int32_t val2, int32_t margin)
{
  if ((val1 > val2 && val1 - val2 > margin)
      || (val2 > val1 && val2 - val1 > margin))
    {
      puts ("ASSERT FAILED\n");
      exit (-1);
    }
}

static inline void 
tests_reports__eat (struct timespec ts) {
  eat_ts (&ts);
}

#define tests_reports__assert(b)                                               \
  if (!(b))                                                                    \
    {                                                                          \
      puts ("ASSERT FAILED\n");                                                \
      puts ((const char *)__FILE__ ":");                                       \
      puts ((const char *)__LINE__);                                           \
      puts ("\n");                                                             \  
      exit (-1);                                                               \      
    }
#endif // _M2_TESTS_REPORTS_H_
