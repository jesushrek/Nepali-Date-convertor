#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "date.h"

typedef struct 
{ 
    int year;
    int month;
    int day;
}Date;

typedef enum 
{ 
    Sunday, 
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    NoDay,
}Edays;

typedef enum 
{ 
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December,
    noMonth,
}Emonth;

typedef enum 
{ 
    Baishakh, 
    Jestha, 
    Ashadh, 
    Shrawan, 
    Bhadra,
    Ashwin,
    Kartik,
    Mangsir,
    Poush,
    Magh,
    Falgun,
    Chaitra,
    noBsMonth,
}BsEmonth;

static const char* bsReturnMonthName( BsEmonth month )
{
    switch(month)
    {
        case Baishakh: return "Baishakh";
        case Jestha: return "Jestha";
        case Ashadh: return "Ashadh";
        case Shrawan: return "Shrawan";
        case Bhadra: return "Bhadra";
        case Ashwin: return "Ashwin";
        case Kartik: return "Kartik";
        case Mangsir: return "Mangsir";
        case Poush: return "Poush";
        case Magh: return "Magh";
        case Falgun: return "Falgun";
        case Chaitra: return "Chaitra";
        default: return "??????";
    }
}
static const char* returnMonthName( Emonth month )
{
    switch(month)
    {
        case January: return "January";
        case February: return "February";
        case March: return "March";
        case April: return "April";
        case May: return "May";
        case June: return "June";
        case July: return "July";
        case August: return "August";
        case September: return "September";
        case October: return "October";
        case November: return "November";
        case December: return "December";
        default: return "??????";
    }
}

static Date returnCurrentTime()
{ 
    time_t t = time(NULL); 
    struct tm *localTime = localtime(&t);

    Date Date = { localTime -> tm_year + 1900, localTime -> tm_mon + 1, localTime -> tm_mday };
    return Date;
}

static const int dayofweek( int y, int m, int d)	/* 1 <= m <= 12, y > 1752 (in the U.K.) */
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if ( m < 2 )
    {
        y -= 1;
    }
    return ( (y + y/4 - y/100 + y/400 + t[m] + d) % 7 ); // Just added -1 here to reduce the index
}

static void printCalender( int days, int start )
{
    printf("Su Mo Tu We Th Fr Sa\n");
    int i = 0;
    for(; i < start; ++i)
        printf("%3c", ' '); 
    for(i = 1; i <= days; ++i)
    {
        printf("%2d ", i);
        if( ( start + i ) % 7 == 0 )
            printf("\n");
    }
}

bool isLeapAd(int year)
{
    return ( year % 4 == 0 ) && ( year % 100 != 0 ) || (year % 400 == 0);
}

static int returnDaysOfMonth( int month, bool isLeapAd )
{
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if( isLeapAd && month == 1 )
        return 29;

    return daysInMonth[month];
}

void printWrapper( int year, int month)
{
    printf("\n%3c %s %d\n", ' ', returnMonthName(month), year);
    printCalender(returnDaysOfMonth(month, isLeapAd(year)),dayofweek(year, month, 1));
    printf("\n");
}

void printYear(int year)
{
    int i = 0;
    for(; i < noMonth; ++i)
        printWrapper(year, i);
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

    days += (long long)day;
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
        int monthDays = returnDaysOfMonth(m, isLeapAd(adYear));
        if(remainingDays < monthDays)
        { 
            converted.year = adYear;
            converted.month = m + 1;
            converted.day = remainingDays - 1;
            break;
        }
        remainingDays -= monthDays;
    }
    return converted;
}

void bsMonthWrapper(int year, int month)
{ 
    printf("\n%3c %s %d\n", ' ', bsReturnMonthName(month - 1), year);
    Date date = bsToAd(year, month, 1);
    printCalender(BSdate[year - bsStartYear][month - 1], dayofweek(date.year, date.month - 1, date.day) );
}

int main()
{ 
    Date current = returnCurrentTime();
    Date BsCurrent = adToBs(current.year, current.month, current.day);
    bsMonthWrapper(BsCurrent.year, BsCurrent.month);
    return 0;
}
