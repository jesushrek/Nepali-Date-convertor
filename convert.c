#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"

static const bool isLeapAd(int year)
{ 
    return (year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0);
}

static const int daysInMonth( int m, bool isLeap )
{ 
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if( m == 1 && isLeap )
        return 29;

    return days[m];
}

static const int bsDaysInYear( int year )
{ 
    if( year > bsUBound.year || year < bsLBound.year )
        return 5000;

    int indexYear = year - bsLBound.year;
    int days = 0;
    int m = 0;

    for(; m < 12; ++m)
        days += bsDaysInMonthsByYear[indexYear][m];

    return days;
}


static const long long bsToDay( Date* day )
{ 
    if( day->year < bsLBound.year || day->year > bsUBound.year )
        return -100;

    long long totalDays = 0;
    int y = bsLBound.year;
    for(; y < day->year; ++y)
        totalDays += bsDaysInYear(y);

    int m = 0;
    for(; m < day->month; ++m)
        totalDays += bsDaysInMonthsByYear[0][m];

    return totalDays + (long long)day->day;
}


static const long long adToDay( Date* day )

{ 
    if( day->year < adLBound.year || day->year > adUBound.year)
        return -100;
    long long totalDays = 0;
    int y = adLBound.year;
    for(; y < day->year; ++y)
        totalDays += isLeapAd(y)? 366 : 365;

    int m = 0;
    for(; m < day->month; ++m)
        totalDays += daysInMonth(m, isLeapAd(day->year));

    totalDays -= dateOffset;
    return totalDays + (long long)day->day;
}

static const Date bsToAd( Date* unit ) 
{ 
    Date converted = { -1, -1, -1 };
    int remainingDays = adToDay(unit);
    int yearIndex = bsLBound.year;
    if( remainingDays < 0 )
        converted;
    while(true)
    { 
        int daysInYear = bsDaysInYear(yearIndex);
        if( remainingDays < daysInYear )
            break;

        remainingDays -= daysInYear;
        ++yearIndex;
    }


    int m = 0;
    for(; m < 12; ++m)
    {
        int daysIn = bsDaysInMonthsByYear[yearIndex - bsLBound.year][m];
        if( remainingDays < daysIn )
        { 
            converted.year = yearIndex;
            converted.month = m;
            converted.day = remainingDays + 1;
            break;
        }
        remainingDays -= daysIn;
    }
    return converted;
}

static const Date adToBs( Date* unit )
{ 
    Date converted = { -1, -1, -1 };
    int remainingDays = bsToDay(unit);
    int yearIndex = adLBound.year;
    if( remainingDays < 0 )
        converted;
    while(true)
    { 
        int daysInYear; 
        if( yearIndex == adLBound.year )
            daysInYear = (365 - dateOffset);
        else 
            daysInYear = isLeapAd(yearIndex)? 366 : 365;

        if( remainingDays < daysInYear )
            break;

        remainingDays -= daysInYear;
        ++yearIndex;
    }


    int m = 0;
    for(; m < 12; ++m)
    {
        int daysIn = daysInMonth(m, isLeapAd(unit->year));
        if( remainingDays < daysIn )
        { 
            converted.year = yearIndex;
            converted.month = m;
            /* fix the off by one */
            converted.day = remainingDays - 1;
            break;
        }
        remainingDays -= daysIn;
    }
    return converted;
}

void printDate( Date* day )
{ 
    printf("%d-%02d-%02d\n", day->year, day->month + 1, day->day);
}
int main( int argc, char* argv[])
{
    if( argc != 5 )
    {
        printf("Usage: %s [ -a | -b ] YYYY MM DD\n", argv[0]);
        printf("  -a: Convert input date (BS) to AD (Gregorian)\n");
        printf("  -b: Convert input date (AD) to BS (Nepali)\n");
        return 1;
    }

    Date inputDate = {};
    inputDate.year = atoi(argv[2]);
    inputDate.month = atoi(argv[3]) - 1;
    inputDate.day = atoi(argv[4]);

    Date converted = { -1, -1, -1 };

    if(strcmp(argv[1], "-a") == 0) 
    {
        converted = bsToAd(&inputDate);
    }
    else if(strcmp(argv[1], "-b") == 0) 
    {
        converted = adToBs(&inputDate);
    }
    else
    {
        printf("Error: Invalid flag '%s'. Use -a or -b.\n", argv[1]);
        return 1;
    }

    if(converted.year != -1)
    {
        printDate(&converted);
    }
    else
    {
        printf("Error: Date is outside of my supported range :(\n");
        return 1;
    }

    return 0;
}
