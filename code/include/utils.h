#ifndef UTILS_H
#define UTILS_H

/**
\file       utils.h
\brief      Utility method file.

This file provides utility functions.
*/

#include <sys/time.h>    //struct timeval

/// @brief Method that calculated the time difference betwen two time values
/// @param end End time
/// @param start Start time
/// @return the time difference in micro-seconds
long time_diff_us(struct timeval end , struct timeval start);

/// @brief Method that returns a number from 0 to the maximum value define in the argument
/// @param max_range Maximum value of the range, must be greater than 0.
/// @return Positive random integer value, or -1 if max_range is invalid
int myrandom(int max_range);

#endif
