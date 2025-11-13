#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct 
{ 
    int year;
    int month;
    int day;
}Data;

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

static Data returnCurrentTime()
{ 
    time_t t = time(NULL); 
    struct tm *localTime = localtime(&t);

    Data data = { localTime -> tm_year, localTime -> tm_mon, localTime -> tm_mday };
    return data;
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

bool isLeapYear(int year)
{
    return ( year % 4 == 0 ) && ( year % 100 != 0 ) || (year % 400 == 0);
}

static int returnDaysOfMonth( int month, bool isLeapYear )
{
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if( isLeapYear && month == 1 )
        return 29;

    return daysInMonth[month];
}

void printWrapper( int year, int month)
{
    printf("\n%3c %s %d\n", ' ', returnMonthName(month), year);
    printCalender(returnDaysOfMonth(month, isLeapYear(year)),dayofweek(year, month, 1));
    printf("\n");
}

void printYear(int year)
{
    int i = 0;
    for(; i < noMonth; ++i)
        printWrapper(year, i);
}

int main() 
{ 
    printWrapper(2025, 1);
    return 0;
}
