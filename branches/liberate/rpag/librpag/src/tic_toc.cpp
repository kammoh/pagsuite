/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

/*
 * Unix C Implementation of the tic() and toc() functions (inspired by matlab)
 *
 * Note: On Linux the programm must be linked with librt-library (-lrt)
 *
 *
 */

#include "tic_toc.h"

#ifdef USE_TIME
#include <iostream>

using namespace std;

struct timespec global_tic_toc_ts_start;
struct timespec global_ts_end;

//wrapper for clock_gettime() which is missing in MAC OS X...
int clock_gettime_wrap(clockid_t clk_id, struct timespec *tp)
{
  (void) clk_id;
  #if _POSIX_TIMERS > 0
  clock_gettime(CLOCK_REALTIME, &tp);
  #else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  tp->tv_sec = tv.tv_sec;
  tp->tv_nsec = tv.tv_usec*1000;
  #endif
  return 1;
}

//tic starts the measurment
timespec tic()
{
    struct timespec ts_start;    
    clock_gettime_wrap(CLOCK_REALTIME,&ts_start);
    global_tic_toc_ts_start = ts_start;
    return ts_start;
}

//toc displays the runtime in seconds
void toc(timespec ts_start)
{
    struct timespec ts_end;
    clock_gettime_wrap(CLOCK_REALTIME,&ts_end);
    
    double time_dif = (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec) * 1E-9;
    cout << "Elapsed time is " << time_dif << " seconds" << endl;
}

void toc()
{
    struct timespec ts_end;
    clock_gettime_wrap(CLOCK_REALTIME,&ts_end);
    
    double time_dif = (ts_end.tv_sec - global_tic_toc_ts_start.tv_sec) + (ts_end.tv_nsec - global_tic_toc_ts_start.tv_nsec) * 1E-9;
    cout << "Elapsed time is " << time_dif << " seconds" << endl;
}    


//toc 'silent' returns the runtime in seconds
double toc_s(timespec ts_start)
{
    struct timespec ts_end;
    clock_gettime_wrap(CLOCK_REALTIME,&ts_end);
    
    double time_dif = (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec) * 1E-9;
    return time_dif;
}


double toc_s()
{
    struct timespec ts_end;
    clock_gettime_wrap(CLOCK_REALTIME,&ts_end);
    
    double time_dif = (ts_end.tv_sec - global_tic_toc_ts_start.tv_sec) + (ts_end.tv_nsec - global_tic_toc_ts_start.tv_nsec) * 1E-9;
    return time_dif;
}    
#endif
