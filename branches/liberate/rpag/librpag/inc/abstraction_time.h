/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef TIME_MESSURE_ABSTRACT_H
#define TIME_MESSURE_ABSTRACT_H

#include <iostream>
#include <string>
#include <sstream>
//#include <ctime>

using namespace std;

#ifdef OS_WIN
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#endif
#ifdef OS_MAC
#include <mach/mach_time.h>
#include <mach/clock.h>
#include <mach/mach.h>
#endif

namespace rpag {

class time_measure {
  private:
    unsigned long long time_start;
    unsigned long long time_stop;
    unsigned long long time_frequency;

    double time_scale;

  public:
    double time_elapsed; // difftime in us

    time_measure() : time_start(0),time_stop(0),time_frequency(0),time_scale(0.0),time_elapsed(0.0) {
    }

    void clear() {
        time_start = 0;
        time_stop = 0;
        time_frequency = 0;
        time_elapsed = 0.0;
        time_scale = 0.0;
    }

    static time_measure &Instance() {
        static time_measure *instance = new time_measure();
        return *instance;
    }

    static void tic() {
        time_measure &i = time_measure::Instance();
        i.clear();
        i.start();
    }

    static double toc() {
        time_measure &i = time_measure::Instance();
        i.stop();
        return i.time_elapsed;
    }

    void start() {
#if OS_WIN
        QueryPerformanceFrequency( (LARGE_INTEGER*) &time_frequency);
        time_scale = (double)time_frequency;
        QueryPerformanceCounter( (LARGE_INTEGER*) &time_start);
#endif
#ifdef OS_LINUX
        timespec t;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t);
        time_start = (t.tv_nsec / 1000) + (t.tv_sec * 1000000);
        //cout << "START_TIME: " << time_start << endl;
#endif
#ifdef OS_MAC
        time_start = mach_absolute_time();
#endif
    }

    void stop() {
#if OS_WIN
        QueryPerformanceCounter( (LARGE_INTEGER*) &time_stop);
        time_elapsed = ((double)(time_stop - time_start) * 1000000.0) / time_scale ;
#endif
#ifdef OS_LINUX
        timespec t;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t);
        time_stop = (t.tv_nsec / 1000) + (t.tv_sec * 1000000);
        //cout << "STOP_TIME: " << time_stop << endl;
        time_elapsed = ((double)(time_stop - time_start));
        //cout << "ELAPSED_TIME: " << time_elapsed << endl;
#endif
#ifdef OS_MAC
        time_stop = mach_absolute_time();
        mach_timebase_info_data_t sTbase;
        mach_timebase_info(&sTbase);
        time_elapsed = ((double)(time_stop - time_start) * (double)sTbase.numer) / ((double)sTbase.denom * 1000.0);
#endif
    }

    string print_time() {
        unsigned long long t_diff = time_elapsed;
        long long us=0,ms=0,s=0,m=0,h=0;
        if( t_diff > 3600000000LL ) {
            h = t_diff / 3600000000LL;
            t_diff = t_diff % 3600000000LL;
        }
        if( t_diff > 60000000LL) {
            m = t_diff / 60000000LL;
            t_diff = t_diff % 60000000LL;
        }
        if( t_diff > 1000000LL) {
            s = t_diff / 1000000LL;
            t_diff = t_diff % 1000000LL;
        }
        if( t_diff > 1000LL) {
            ms = t_diff / 1000LL;
            t_diff = t_diff % 1000LL;
        }
        us = t_diff;
        stringstream time_string;
        if(h>0) time_string << h << "h : ";
        if(m>0) time_string << m << "m : ";
        if(s>0) time_string << s << "s : ";
        if(ms>0) time_string << ms << "ms : ";
        time_string << us << "us";
        return time_string.str();
    }


};

}

#endif // TIME_MESSURE_ABSTRACT_H
