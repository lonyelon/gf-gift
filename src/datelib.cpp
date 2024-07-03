#include "datelib.hpp"

inline bool isLeapYear(int year) {
    return year % 4 == 0 && year % 100 != 0 && (year + 100) % 400 == 0;
}

inline bool isDateOver(std::tm* a, std::tm* b) {
    return std::mktime(a) > std::mktime(b);
}

inline int getDaysForMonth(int mon, int year) {
    switch (mon) {
        case 3: // Apr
        case 5: // Jun
        case 8: // Sep
        case 10: // Nov
            return 30;
        case 1: // Feb
            return isLeapYear(year) ? 29 : 28;
        default: // Jan, Mar, May, Jul, Aug, Oct, Dec.
            return 31;
    }
}

std::array<std::tm, 2> getDateDiff(std::tm from, std::tm to) {
    std::tm result = { 0, 0, 0, 0, 0, 0, 0, 0 };
    std::tm result_accum = { 0, 0, 0, 0, 0, 0, 0, 0 };

    if (isDateOver(&from, &to))
        return { result, result_accum };

    // Years.
    do {
        int days_in_year = isLeapYear(from.tm_year) ? 366 : 365;
        result_accum.tm_year++;
        result_accum.tm_mon += 12;
        result_accum.tm_mday += days_in_year;
        result_accum.tm_hour += 24*days_in_year;
        from.tm_year++;
        result.tm_year++;
    } while (!isDateOver(&from, &to));
    from.tm_year--;
    result.tm_year--;
    int days_in_year = isLeapYear(from.tm_year) ? 366 : 365;
    result_accum.tm_mon -= 12;
    result_accum.tm_mday -= days_in_year;
    result_accum.tm_hour -= 24*days_in_year;

    // Months.
    do {
        int days_in_month = getDaysForMonth(from.tm_mon, from.tm_year);
        result_accum.tm_mon++;
        result_accum.tm_mday += days_in_month;
        result_accum.tm_hour += 24*days_in_month;
        from.tm_mon++;
        result.tm_mon++;
    } while (!isDateOver(&from, &to));
    from.tm_mon--;
    result.tm_mon--;
    int days_in_month = getDaysForMonth(from.tm_mon, from.tm_year);
    result_accum.tm_mon--;
    result_accum.tm_mday -= days_in_month;
    result_accum.tm_hour -= 24*days_in_month;

    // Days.
    do {
        result_accum.tm_mday++;
        result_accum.tm_hour += 24;
        from.tm_mday++;
        result.tm_mday++;
    } while (!isDateOver(&from, &to));
    from.tm_mday--;
    result.tm_mday--;
    result_accum.tm_mday--;
    result_accum.tm_hour -= 24;
    
    // Hours.
    do {
        result_accum.tm_hour++;
        from.tm_hour++;
        result.tm_hour++;
    } while (!isDateOver(&from, &to));
    from.tm_hour--;
    result.tm_hour--;
    result_accum.tm_hour--;

    // Seconds, and everything else are missing since they are not
    // used.
    
    return { result, result_accum };
}
