#include <sys/time.h>    //struct timeval
#include <stdlib.h>      //rand, srand, RAND_MAX
#include <stdio.h>       //fprintf, stderr
#include <time.h>        //time

#include "utils.h"

long time_diff_us(struct timeval end , struct timeval start)
{
    return (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
}

int myrandom(int max_range)
{
    if(max_range <= 0 || max_range > RAND_MAX)
    {
        fprintf(stderr, "ERROR: Invalid argument max_range (=%d) of function myrandom", max_range);
        return -1;
    }
    srand(time(NULL));  //use current time as seed for random generator
    return rand() % max_range;
}
