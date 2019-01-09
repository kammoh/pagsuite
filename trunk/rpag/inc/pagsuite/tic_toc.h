/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef TIC_TOC
#define TIC_TOC

#include <time.h>

#include <sys/time.h>
#include <iostream>

using namespace std;

//#define USE_TIME //enable time profiling

#ifdef USE_TIME

namespace PAGSuite
{
  extern struct timespec global_tic_toc_ts_start;
  extern struct timespec global_ts_end;

  typedef enum {
    CLOCK_REALTIME,
    CLOCK_MONOTONIC,
    CLOCK_PROCESS_CPUTIME_ID,
    CLOCK_THREAD_CPUTIME_ID
  } clockid_t;

  int clock_gettime_wrap(clockid_t clk_id, struct timespec *tp);

  //tic starts the measurment
  timespec tic();

  //toc displays the runtime in seconds
  void toc(timespec ts_start);
  void toc();

  //toc 'silent' returns the runtime in seconds
  double toc_s(timespec ts_start);
  double toc_s();

//toc for debug purposes (adds filname and line number)
#define tocdbg() \
    clock_gettime_wrap(CLOCK_REALTIME,&global_ts_end); \
    cout << "timstamp: " << __FILE__ << " line " << __LINE__ << ": " << ((global_ts_end.tv_sec - global_tic_toc_ts_start.tv_sec) * 1000 + (global_ts_end.tv_nsec - global_tic_toc_ts_start.tv_nsec) * 1E-6) << " ms" << endl; \
    tic();

//    cout << "timstamp: " << __FILE__ << " line " << __LINE__ << ": " << (global_ts_end.tv_sec - global_tic_toc_ts_start.tv_sec) * 1000 + (global_ts_end.tv_nsec - global_tic_toc_ts_start.tv_nsec) * 10.0E-6 << " ms" << endl;
}

#endif


#endif //TIC_TOC

