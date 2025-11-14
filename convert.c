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

static const long long bsCountDaysFromRef(int year, int month, int day)
{ 
    long long days = 0;
    int y = 0;
    for(; y < (year - bsStartYear); ++y)
    { 
        int m = 0;
        for(;m < 12; ++m)
            days += BSdate[y][m];
    }

    int mo = 0;
    for(; mo < (month - 1); ++mo)
        days += BSdate[year - bsStartYear][mo];
    //i am off by one for some reason

    days += (long long)day - 1;
    return days;
}

static const Date adToBs( int year, int month, int day )
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

static const Date bsToAd(int year, int month, int day)
{ 
    long long remainingDays = bsCountDaysFromRef(year, month, day);
    int adYear = adStartYear;
    while(adYear <= adEndYear)
    { 
        int daysInCurrentYear = (adYear == adStartYear)? -(adStartDay) : 0;
        int m = (adYear == adStartYear)? 3 : 0;
        for(; m < 12; ++m)
            daysInCurrentYear += returnDaysOfMonth(m, isLeapAd(adYear));
        if(remainingDays < daysInCurrentYear)
            break;
        remainingDays -= daysInCurrentYear;
        ++adYear;
    }
    Date converted = { 0, 0, 0 };
    int m = 0;
    for(; m < 12; ++m)
    { 
        int daysInMonth = returnDaysOfMonth(m, isLeapAd(adYear));
        if(remainingDays < daysInMonth)
        { 
            converted.year = adYear;
            converted.month = m + 1;
            converted.day = remainingDays;
            break;
        }
    }
    return converted;
}

int main()
{ 
    Date day = bsToAd(2063, 10, 13);
    printf(" 2063 10 13 converts to %d %d %d", day.year, day.month, day.day);
}
