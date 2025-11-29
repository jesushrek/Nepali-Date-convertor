#include "date.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const bool isLeapAd(int year)
{
    return (year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0);
}

static const int daysInMonth(int m, bool isLeap)
{ 
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if( m == 1 && isLeap )
        return 29;

    return days[m];
}

static const int bsDaysInYear( int year )
{ 
    int indexYear = (year - bsLBound.year);
    int days = 0;
    int m = 0;

    for(; m < 12; ++m)
        days += bsDaysInMonthsByYear[indexYear][m];

    return days;
}

static const long long bsToDay( Date* bsDate )
{ 
    long long totalDays = 0;
    int y = bsLBound.year;
    for(; y < bsDate->year; ++y)
        totalDays += bsDaysInYear(y);

    const int currentIndex = (bsDate->year - bsLBound.year);

    int m = 0;
    for(; m < bsDate->month; ++m)
        totalDays += bsDaysInMonthsByYear[currentIndex][m];

    //fix the off by one
    return totalDays + (long long)bsDate->day - 1;
}

static const long long adToDay( Date* adDate )
{ 
    long long totalDays = 0;
    int y = adLBound.year;
    for(; y < adDate->year; ++y)
        totalDays += isLeapAd(y)? 366 : 365;

    int m = 0;
    for(; m < adDate->month; ++m)
        totalDays += daysInMonth(m, isLeapAd(adDate->year));

    totalDays -= dateOffset;
    return totalDays + (long long)adDate->day;
}

static const Date bsToAd( Date* bsDate )
{
    Date converted = { -1, -1, -1 };
    long long remainingDays = bsToDay(bsDate);
    if(remainingDays < 0)
        return converted;

    int yearIndex = adLBound.year;
    while(true)
    { 
        int daysInYear = 0;
        if( yearIndex == adLBound.year)
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
        int daysIn = daysInMonth(m, isLeapAd(yearIndex));
        if( remainingDays < daysIn )
        { 
            converted.year = yearIndex;
            converted.month = m;
            converted.day = remainingDays;
            break;
        }
        remainingDays -= daysIn;
    }

    return converted;
}

static const Date adToBs( Date* adDate )
{
    Date converted = { -1, -1, -1 };
    int remainingDays = adToDay(adDate);

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
            //fix another off by one 
            converted.day = remainingDays + 1;
            break;
        }
        remainingDays -= daysIn;
    }
    return converted;
}

static const Error isValidDateAd( const Date* adDate )
{   
    if( adDate->month > 11 || adDate->month < 0 )
        return notValidMonth;

    if(daysInMonth(adDate->month, isLeapAd(adDate->year)) < adDate->day)
        return notValidDay;

    if( adDate->day < 0 )
        return notValidDay;


    if( adDate->year > adUBound.year || adDate->year < adLBound.year )
        return outOfBounds;

    if( adDate->year == adLBound.year )
    { 
        if( adDate->month < adLBound.month )
            return outOfBounds;

        if( adDate->month == adLBound.month )
            if(adDate->day < adLBound.day)
                return outOfBounds;
    }

    if( adDate->year == adUBound.year )
    { 
        if( adDate->month > adUBound.month )
            return outOfBounds;

        if( adDate->month == adUBound.month )
            if(adDate->day > adUBound.day)
                return outOfBounds;
    }

    return noError;
}

static const Error isValidDateBs( const Date* bsDate )
{   
    const int index = (bsDate->year - bsLBound.year);

    if( bsDate->month > 11 || bsDate->month < 0 )
        return notValidMonth;

    if(bsDaysInMonthsByYear[index][bsDate->month] < bsDate->day)
        return notValidDay;

    if( bsDate->day < 0 )
        return notValidDay;


    if( bsDate->year > bsUBound.year || bsDate->year < bsLBound.year )
        return outOfBounds;

    if( bsDate->year == bsLBound.year )
    { 
        if( bsDate->month < bsLBound.month )
            return outOfBounds;

        if( bsDate->month == bsLBound.month )
            if(bsDate->day < bsLBound.day)
                return outOfBounds;
    }

    if( bsDate->year == bsUBound.year )
    { 
        if( bsDate->month > bsUBound.month )
            return outOfBounds;

        if( bsDate->month == bsUBound.month )
            if(bsDate->day > bsUBound.day)
                return outOfBounds;
    }

    return noError;
}

static const Error validate(Date *date, bool isAd)
{ 
    switch((int)isAd)
    { 
        case 1: 
            return isValidDateAd(date);
        case 2:
            return isValidDateAd(date);
        default:
            return noError;
    }
    return noError;
}

void printDate( const Date* unit )
{ 
    printf("%d-%02d-%02d", unit->year, unit->month + 1, unit->day);
}

int main( int argc, char* argv[])
{
    if( argc != 5 )
    {
        printf("Usage: %s [ -atb | -bta ] YYYY MM DD\n", argv[0]);
        printf("  -bta: Convert input date (BS) to AD (Gregorian)\n");
        printf("  -atb: Convert input date (AD) to BS (Nepali)\n");
        return 1;
    }
    Date inputDate = {};
    inputDate.year = atoi(argv[2]);
    inputDate.month = atoi(argv[3]) - 1;
    inputDate.day = atoi(argv[4]);

    Error catch = noError;
    if(strcmp(argv[1], "-bta") == 0)
    { 
        catch = validate(&inputDate, false);
    }
    if(strcmp(argv[1], "-atb") == 0)
    { 
        catch = validate(&inputDate, true);
    }

    switch(catch) 
    {
        case outOfBounds:
            printf("That Date is out of bounds.\n");
            printf("For AD can only convert from %d %d %d AD to %d %d %d AD\n", adLBound.year, adLBound.month, adLBound.day, 
                    adUBound.year, adUBound.month,adUBound.day);
            printf("For Bs can only convert from %d %d %d BS to %d %d %d BS\n", bsLBound.year, bsLBound.month, bsLBound.day,
                    bsUBound.year, bsUBound.month, bsUBound.day);
            return 1;
        case notValidYear:
            printf("That year is not valid!\n");
            return 1;
        case notValidMonth: 
            printf("That month is not valid!\n");
            return 1;
        case notValidDay:
            printf("That day is not valid!\n");
            return 1;
        default:
            break;
    }

    Date converted = { -1, -1, -1 };
    if(strcmp(argv[1], "-bta") == 0)
    {
        converted = bsToAd(&inputDate);
    }
    else if(strcmp(argv[1], "-atb") == 0)
    {
        converted = adToBs(&inputDate);
    }
    else
    {
        printf("\nError: Invalid flag '%s'. Use -atb or -bta.\n", argv[1]);
        return 1;
    }

    if(converted.year != -1)
    {
        printDate(&converted);
        printf("\n");
    }

    return 0;
}

