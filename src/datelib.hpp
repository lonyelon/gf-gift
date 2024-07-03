#pragma once

#include <array>
#include <chrono>

inline bool isLeapYear(int year);

// Compare dates a and b and check if a's time is over b's time. 
inline bool isDateOver(std::tm* a, std::tm* b);

// Return the amount of days in a given month in a given year.
inline int getDaysForMonth(int mon, int year);

// Return two std::tm, one with the difference between the two dates and the
// other being the same but accumulative.
//
// For example, if one date is 2023-06-12 and the other is 2024-08-20, the
// first value will hold 1-02-08 (the difference between each date component),
// and the second will hold 1-14-434 (the difference in each measurement).
// 
// This is not how the std::tm was intended to be used but it allows us to not
// declare another struct for the process.
std::array<std::tm, 2> getDateDiff(std::tm from, std::tm to);
