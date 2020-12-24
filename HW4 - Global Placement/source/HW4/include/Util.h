#ifndef UTIL_H
#define UTIL_H
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <sys/resource.h>

using namespace std;

/////////////////////////////////////////////////
// debug macros
/////////////////////////////////////////////////

// ASSERTCRASH function (1: crash 0: infinite loop)
#if 1
#define ASSERTCRASH() *(int *)(uintptr_t)0xbbadbeef = 0
#else
#define ASSERTCRASH() \
    do                \
    {                 \
    } while (true)
#endif

// ASSERT function
#define ASSERT(cond)                                        \
    do                                                      \
    {                                                       \
        if (cond)                                           \
            break;                                          \
        fprintf(stderr, "%s:%d: failed assertion \"%s\"\n", \
                __FILE__, __LINE__, #cond);                 \
        ASSERTCRASH();                                      \
    } while (false)

// DEBUGCODE macro
#define DEBUGCODE(code) code

/////////////////////////////////////////////////
// utility functions
/////////////////////////////////////////////////
// string
string format(const char *fmt, ...);
string itos(int num);
string dtos(double num);

// memory
double getPeakMemoryUsage();
double getCurrentMemoryUsage();

// filename
string getFileBasename(string filePathName);
string getFileName(string filePathName);

////////////////////////////////////////////////////////////
// record  two point
class CPoint
{

public:
    //friend double Distance( const CPoint& p1,  const CPoint& p2 )
    static double Distance(const CPoint &p1, const CPoint &p2)
    {
        return fabs(p1.x - p2.x) + fabs(p1.y - p2.y);
    }

    // 2006-08-05 (donnie)
    //friend double XDistance( const CPoint& p1,  const CPoint& p2 )
    static double XDistance(const CPoint &p1, const CPoint &p2)
    {
        double dis_x = fabs(p1.x - p2.x);
        double dis_y = fabs(p1.y - p2.y);
        if (dis_x > dis_y)
            return dis_x - dis_y + dis_y * sqrt(2);
        else
            return dis_y - dis_x + dis_x * sqrt(2);
    }

    CPoint(){};
    CPoint(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    void Print()
    {
        cout << "(" << x << "," << y << ")";
    }
    double x, y;
};

inline double seconds()
{

    rusage time;
    getrusage(RUSAGE_SELF, &time);
    //return (double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec);	// user time

    return (double)(1.0 * time.ru_utime.tv_sec + 0.000001 * time.ru_utime.tv_usec + // user time +
                    1.0 * time.ru_stime.tv_sec + 0.000001 * time.ru_stime.tv_usec); // system time

#if 0
    struct timeval tv;	// seconds from 00:00am
    struct timezone tz;
    gettimeofday( &tv, &tz );
    return (double)( tv.tv_sec + 0.000001*tv.tv_usec );
    // clock() loop is about 72min. (or 4320 sec)
    //return double(clock())/CLOCKS_PER_SEC;
#endif
}

#endif // UTIL_H
