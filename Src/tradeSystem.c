/*
 * source file tradeSystem.c
 *
 * Algorithm for a channel breakout system
 *
 * Becky Engley and Martin Neal
 * May 2009
 *
 */

#include <stdio.h>
#include <string.h>
#include "tradeSystem.h"
#include "database.h"

extern commod comDatabase[NUMCOMMODS];

/* ------------------------------- utility prototypes ---------------------------- */

int lookupTypeTick(commod* pcomCommod);



/* ----------------------------- trade system function --------------------------- */

double tradeSystem(char* szName, int iYear, int iEntryWindow, int iTrailStopWindow, int iStopLossWindow,
    char* szEntryDate, char* szNoEntryDate, char* szExitDate)
{

    double* adOpen;
    double* adClose;
    double* adLow;
    double* adHigh;
    char** aszDates;
    int iSize;

    double* adEntryChannel;
    double* adTrailStopChannel;
    double* adStopLossChannel;
    
    commod comCommodity;

    int t;
    int iStartDay;
    int fDone = 0;

    int iError;

    //set commodity values
    comCommodity.szName = szName;
    comCommodity.iYear = iYear;
    iError = lookupTypeTick(&comCommodity);
    if(iError) return iError;
    
    //get trade data
    //iError = generateTradeData(comCommodity, &pdLow, &pdHigh, &


    //get channels


    return 0;
}

/* -------------------------------- utility functions ---------------------------- */


int lookupTypeTick(commod* pcomCommod)
{
    // Looks up the type, tick size and tick value for this commodity. Returns 0 on
    // success; -1, if the commodity was not found.

    int i = 0; 
    int fFound = 0;

    while(!fFound && i < NUMCOMMODS)
    {
        if(strncmp(pcomCommod->szName, comDatabase[i].szName, 6) == 0)
        {
            pcomCommod->iType = comDatabase[i].iType;
            pcomCommod->dTickVal = comDatabase[i].dTickVal;
            pcomCommod->dTickSize = comDatabase[i].dTickSize;
            fFound = 1;
        }

        i++;
    }

    if(!fFound)
    {
        fprintf(stderr, "tradeSystem: No such commodity\n");
        return -1;
    }

    return 0;
}
