#include <stdio.h>
#include <stdlib.h>
#include "commodities.h"

#define NUMARGS 9

int main(int argc, char** argv)
{
    char* szName;
    int iYear;
    int iEntryWindow;
    int iTrailStopWindow;
    int iStopLossWindow;
    char* szEntryDate;
    char* szNoEntryDate;
    char* szExitDate;

    double dProfit;

    if(argc != NUMARGS)
    {
        fprintf(stderr, "Usage: %s <commodity name> <year> <entry window> <trail stop window> \
            <stop loss window> <entry date> <no entry date> <exit date>\n", argv[0]);
        exit(1);
    }

    szName = (char*) argv[1];
    iYear = (int) argv[2];
    iEntryWindow = (int) argv[3];
    iTrailStopWindow = (int) argv[4];
    iStopLossWindow = (int) argv[5];
    szEntryDate = (char*) argv[6];
    szNoEntryDate = (char*) argv[7];
    szExitDate = (char*) argv[8];

    dProfit = tradeSystem(szName, iYear, iEntryWindow, iTrailStopWindow, iStopLossWindow,
        szEntryDate, szNoEntryDate, szExitDate);

    printf("commodity name: %s\nyear: %d\nentry window: %d\n", szName, iYear, iEntryWindow);
    printf("trail stop window: %d\nstop loss window: %d\n", iTrailStopWindow, iStopLossWindow);
    printf("entry date: %s\nno entry date: %s\nexit date: %s\n", szEntryDate, szNoEntryDate, szExitDate);
    printf("profit earned: %f\n", dProfit);

    return 0;
}
