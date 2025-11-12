#include <stdio.h>
#include <stdbool.h>
#include "date.h"

typedef struct 
{ 
    int year;
    int month;
    int day;

} Date;

static const bool isLeapAd( int year ) { return (year % 4 == 0 ) && (year % 100 != 0) || (year % 400 == 0); }

static const int returnDaysOfMonth( int month, bool isLeapYear )
{
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if( isLeapYear && month == 1 )
        return 29;

    return daysInMonth[month];
}

static const long long countDaysFromRef( int year, int month, int day )
{ 
    long long days = 0;

    int y = adStartYear;
    for(; y < year; ++y)
        days += isLeapAd(y)? 366 : 365;

    int m = 0;
    for(; m < (month - 1); ++m)
        days += returnDaysOfMonth(m, isLeapAd(year));

    days += day;

    return days;
}

const static Date adToBs( int year, int month, int day )
{ 
    long long remainingDays = countDaysFromRef(year, month, day) - daysFromRef;
    int bsYear = bsStartYear;
    int bsYearIndex = 0;

    while(bsYearIndex < sizeof(BSdate) / sizeof(BSdate[0]) )
    { 
        int daysInCurrentYear = 0;
        int m = 0;
        for(; m < 12; ++m)
            daysInCurrentYear += BSdate[bsYearIndex][m];
        if( remainingDays < daysInCurrentYear )
            break;
        // shrinking the sample
        remainingDays -= daysInCurrentYear;
        ++bsYear;
        ++bsYearIndex;
    }

    Date converted = { 0 , 0 , 0 };

    int m = 0;
    for( ; m < 12; ++m )
    { 
        int daysInMonths = BSdate[bsYearIndex][m];
        if( remainingDays < daysInMonths )
        { 
            converted.year = bsYear;
            converted.month = m + 1;
            converted.day = remainingDays + 1;
            break;
        }
        remainingDays -= daysInMonths;
    }

    return converted;
}

int main()
{ 
    Date date = adToBs(2025, 11, 12);
    printf("2025, 11, 12 converts to %d, %d, %d", date.year, date.month, date.day);
    return 0;
}
