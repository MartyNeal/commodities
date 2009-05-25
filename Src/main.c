#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "commodities.h"

#define NUMARGS 9

int main(int argc, char** argv)
{
    /*
    char* szName;
    double dPercent;
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
        fprintf(stderr, "Usage: %s <commodity name> <year | percent> <entry window> <trail stop window> "
            "<stop loss window>\n\t<entry date> <no entry date> <exit date>\n", argv[0]);
        exit(1);
    }

    szName = (char*) argv[1];
    dPercent = atof(argv[2]);
    iYear = atoi(argv[2]);
    iEntryWindow = atoi(argv[3]);
    iTrailStopWindow = atoi(argv[4]);
    iStopLossWindow = atoi(argv[5]);
    szEntryDate = (char*) argv[6];
    szNoEntryDate = (char*) argv[7];
    szExitDate = (char*) argv[8];

    if(dPercent <= 1 && dPercent >= 0) //assume percentage
    {
        printf("Percent value in correct range, calling tradeSystemData -----------\n");
        dProfit = tradeSystemData(szName, dPercent, iEntryWindow, iTrailStopWindow, iStopLossWindow,
            szEntryDate, szNoEntryDate, szExitDate);
    }
    else
    {
        printf("Calling tradeSystem -----------------------------------------------\n");
        dProfit = tradeSystem(szName, iYear, iEntryWindow, iTrailStopWindow, iStopLossWindow,
            szEntryDate, szNoEntryDate, szExitDate);
    }

    printf("in main.c ------------------------\n");

    printf("commodity name: %s\nyear: %d\nentry window: %d\n", szName, iYear, iEntryWindow);
    printf("trail stop window: %d\nstop loss window: %d\n", iTrailStopWindow, iStopLossWindow);
    printf("entry date: %s\nno entry date: %s\nexit date: %s\n", szEntryDate, szNoEntryDate, szExitDate);
    printf("profit earned: %f\n", dProfit);
    */

    int i;
    int sumR;
    int sumS;
    int j;
    int iIterations = 200;
    time_t tTime;
    srand(time(&tTime));

    printf("i\tRandom\tSimulated\n");
    for(i = 0; i < iIterations; i += 10)
    {
        sumR = 0;
        sumS = 0;
        for(j = 0; j < 100; j++)
        {
            sumR += RandomAlgorithm(i,"Jun_CL",6,UNIFORM);
            sumS += SimulatedAnnealing(i,"Jun_CL",6,NORMAL);
        }
        printf("%d\t%d\t%d\n",i,sumR/j,sumS/j);
    }
    return 0;
}
