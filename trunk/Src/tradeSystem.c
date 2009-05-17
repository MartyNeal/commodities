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
#include <stdlib.h>
#include "tradeSystem.h"
#include "database.h"

extern commod comDatabase[NUMCOMMODS];

/* ------------------------------- utility prototypes ---------------------------- */

int nameLookup(char* szName);
// Returns the key into the database for this name, or ERRVAL if the key doesn't exist.

int lookupTypeTick(commod* pcomCommod);
// Looks up the type, tick size and tick value for this commodity. Returns 0 on
// success; ERRVAL, if the commodity was not found.

int computeStartDay(int iWinA, int iWinB, int iWinC);
// Computes the start day to be the maximum of the three windows.

int checkForNAValues(int t, double* adEntryChannel, double* adTrailStopChannel, double* adStopLossChannel);
// Checks for NA values in the channels, past the start day.

int checkEnter(int iType, double dEntryChnl, double dLow, double dHigh, char* szEntryDate,
    char* szNoEntryDate, char* szCurrDate);
// Determines whether we should enter the trade. Checks if we're within the entry
// window. Compares the entry channel with the low if we are selling short.
// Compares the entry channel with the high if we are selling long.

int inEntryWindow(char* szEntryDate, char* szNoEntryDate, char* szCurrDate);
// Returns -1 if before the entry window, 0 if in the entry window, 1, if passed.

double computeEntryPoints(int iType, double dTickSize, double dEntryChnl, double dOpen);
// Computes the entry price, by comparing the open and entryChannel values.

int checkExit(int iType, double dStopLoss, double dTrailStop, double dLow, double dHigh,
    char* szExitDate, char* szCurrDate, double dEntryPoints);
// Determines whether we should exit the trade. Checks if we've passed the exit date.
// Compares the closer of the trail stop and stop loss with the high if we're selling
// short; and with the low if we're selling long.

double computeExitPoints(int iType, double dTickSize, double dStopLoss, double dTrailStop,
    double dOpen, double dClose, int iExit);
// Computes the exit points by comparing open to the closer of the stop loss and the
// trail stop.

double computeProfit(int iType, double dEntryPoints, double dExitPoints, double dTickVal, double dTickSize);
// Computes the total profit earned.

/* ----------------------------- trade system functions --------------------------- */

double tradeSystem(char* szName, int iYear, int iEntryWindow, int iTrailStopWindow,
    int iStopLossWindow, char* szEntryDate, char* szNoEntryDate, char* szExitDate)
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
    int fDone = 0;
    int iError;
    int iExit;
    int t;
    int i;
    int fStartDayOn = 0; //we aren't on at the start of the day yet

    double dEntryPoints = 0;
    double dExitPoints = 0;
    double dStopLoss;

    double dProfit;

    //set commodity values
    comCommodity.szName = szName;
    comCommodity.iYear = iYear;
    iError = lookupTypeTick(&comCommodity);
    if(iError == ERRVAL) return iError;

    //get trade data
    iError = generateTradeData(comCommodity, &adLow, &adHigh, &adOpen, &adClose, &aszDates, &iSize);
    if(iError == ERRVAL) return iError;

    //get channels
    iError = generateChannels(comCommodity, adLow, adHigh, iEntryWindow, iTrailStopWindow, iStopLossWindow,
        iSize, &adEntryChannel, &adTrailStopChannel, &adStopLossChannel);
    if(iError == ERRVAL)
    {
        free(adLow);
        free(adHigh);
        free(adOpen);
        free(adClose);
        free(aszDates);
        return iError;
    }

/*    printf("Date\t Open\tHigh\tLow\tClose\tEntry\tTrail\tStop\n");
    for(i = 0; i < iSize; i++)
    {
        printf("%s %.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\n",
               aszDates[i],
               adOpen[i],
               adHigh[i],
               adLow[i],
               adClose[i],
               adEntryChannel[i],
               adTrailStopChannel[i],
               adStopLossChannel[i]);
    }*/

    //calculate the start day
    t = computeStartDay(iEntryWindow, iTrailStopWindow, iStopLossWindow);

    while(!fDone && t < iSize)
    {
        //check for NA values
        iError = checkForNAValues(t, adEntryChannel, adTrailStopChannel, adStopLossChannel);
        if(iError == ERRVAL) return iError;

        //if we haven't entered
        if(!fStartDayOn)
        {
            //check if we should enter
            if(checkEnter(comCommodity.iType, adEntryChannel[t-1], adLow[t], adHigh[t], szEntryDate,
                szNoEntryDate, aszDates[t]))
            {
                //set flag to on, compute entry price and stop loss
                fStartDayOn = 1;
                dEntryPoints += computeEntryPoints(comCommodity.iType, comCommodity.dTickSize,
                    adEntryChannel[t-1], adOpen[t]);
                dStopLoss = adStopLossChannel[t-1];
            }

            //if we didn't enter
            else
            {
                //set done flag, if past entry window
                if(inEntryWindow(szEntryDate, szNoEntryDate, aszDates[t]) > 0)
                    fDone = 1;
            }
        }

        //if we have entered
        else
        {
            iExit = checkExit(comCommodity.iType, dStopLoss, adTrailStopChannel[t-1],
                adLow[t], adHigh[t], szExitDate, aszDates[t], dEntryPoints);

            //check if we should exit
            if(iExit)
            {
                //set flag to off, compute exit price
                fStartDayOn = 0;
                dExitPoints += computeExitPoints(comCommodity.iType, comCommodity.dTickSize,
                    dStopLoss, adTrailStopChannel[t-1], adOpen[t], adClose[t], iExit);

                //set done flag, if past entry window
                if(inEntryWindow(szEntryDate, szNoEntryDate, aszDates[t]) > 0)
                    fDone = 1;
            }

        }

        t++;
    }

    if(!fDone)
        fprintf(stderr, "tradeSystem: Never exited final trade\n");

    //compute profit!
    dProfit = computeProfit(comCommodity.iType, dEntryPoints, dExitPoints, 
        comCommodity.dTickVal, comCommodity.dTickSize);

    //free everything!
    for (i = 0; i < iSize; i++)
    {
        if(aszDates[i] != NULL) free(aszDates[i]);
    }


    free(adOpen);
    free(adClose);
    free(adLow);
    free(adHigh);
    free(aszDates);
    free(adEntryChannel);
    free(adTrailStopChannel);
    free(adStopLossChannel);

    return dProfit;
}

double tradeSystemData(char* szName, double dPercentData, int iEntryWindow, int iTrailStopWindow,
    int iStopLossWindow, char* szEntryDate, char* szNoEntryDate, char* szExitDate)
{

    double dTotalProfit = 0;
    int iBaseYear;
    int iTotalYears;
    int iKey;
    int iNumYears;
    int iYear;
    int i = 0;

    if(dPercentData < 0 || dPercentData > 1)
    {
        fprintf(stderr, "tradeSystem: Invalid data percentage\n");
        return ERRVAL;
    }

    iKey = nameLookup(szName);
    if(iKey < 0) return ERRVAL;

    iBaseYear = comDatabase[iKey].iYear;
    iTotalYears = comDatabase[iKey].iNumYears;

    iNumYears = iTotalYears * dPercentData;

    for(i = 0; i < iNumYears; i++)
    {
        iYear = iBaseYear + i;

        dTotalProfit += tradeSystem(szName, iYear, iEntryWindow, iTrailStopWindow,
            iStopLossWindow, szEntryDate, szNoEntryDate, szExitDate);
    }

    return dTotalProfit;

}

/* -------------------------------- utility functions ---------------------------- */


int nameLookup(char* szName)
{
    // Returns the key into the database for this name, or ERRVAL if the key doesn't
    // exist.

    int i = 0;
    int iKey = ERRVAL;

    while((iKey == ERRVAL) && (i < NUMCOMMODS))
    {
        if(strncmp(szName, comDatabase[i].szName, MAX_NAME_LEN) == 0)
            iKey = i;
        i++;
    }

    if(iKey == ERRVAL)
        fprintf(stderr, "tradeSystem: No such commodity\n");

    return iKey;
}

int lookupTypeTick(commod* pcomCommod)
{
    // Looks up the type, tick size and tick value for this commodity. Returns 0
    // on success; ERRVAL, if the commodity was not found.

    int iKey = nameLookup(pcomCommod->szName);
    if(iKey < 0) return iKey;

    pcomCommod->iType = comDatabase[iKey].iType;
    pcomCommod->dTickVal = comDatabase[iKey].dTickVal;
    pcomCommod->dTickSize = comDatabase[iKey].dTickSize;

    return (0);
}

int computeStartDay(int iWinA, int iWinB, int iWinC)
{
    // Computes the start day to be the maximum of the three windows.

    if(iWinA >= iWinB)
    {
        if(iWinA >= iWinC) return iWinA;
        else return iWinC;
    }
    else
    {
        if(iWinB >= iWinC) return iWinB;
        else return iWinC;
    }
}

int checkForNAValues(int t, double* adEntryChannel, double* adTrailStopChannel, double* adStopLossChannel)
{
    // Checks for NA values in the channels, past the start day.

    int iErr = 0;

    if(adEntryChannel[t-1] == ERRVAL || adTrailStopChannel[t-1] == ERRVAL || adStopLossChannel[t-1] == ERRVAL)
    {
        iErr = ERRVAL;
        fprintf(stderr, "tradeSystem: found an NA value where there shouldn't be one.\n");
    }

    return iErr;
}

int checkEnter(int iType, double dEntryChnl, double dLow, double dHigh, char* szEntryDate, char* szNoEntryDate,
    char* szCurrDate)
{
    // Determines whether we should enter the trade. Checks if we're
    // within the entry window. Compares the entry channel with the
    // low if we are selling short. Compares the entry channel with
    // the high if we are selling long.

    if(inEntryWindow(szEntryDate, szNoEntryDate, szCurrDate) == 0)
    {
        //for short, compare low to entry channel
        if(iType == SHORT)
        {
            if(dLow < dEntryChnl) return 1; //enter
            else return 0; //do not enter
        }
        //for long, compare high to entry channel
        else
        {
            if(dHigh > dEntryChnl) return 1; //enter
            else return 0; //do not enter
        }
    }

    //not within entry window
    else return 0; //do not enter

}

int inEntryWindow(char* szEntryDate, char* szNoEntryDate, char* szCurrDate)
{
    // Returns -1 if before the entry window, 0 if in the entry window, 1, if passed.
    
    if(strncmp(szCurrDate, szEntryDate, DATE_LEN) <= 0) return -1;
    else if(strncmp(szCurrDate, szNoEntryDate, DATE_LEN) <= 0) return 0;
    else return 1;
}


double computeEntryPoints(int iType, double dTickSize, double dEntryChnl, double dOpen)
{
    // Computes the entry points, by comparing the open and entryChannel
    // values.

    double dPoints;

    //compute short entry points
    if(iType == SHORT)
    {
        if(dOpen < dEntryChnl) dPoints = dOpen;
        else dPoints = dEntryChnl - dTickSize;
    }

    //compute long entry points
    else
    {
        if(dOpen > dEntryChnl) dPoints = dOpen;
        else dPoints = dEntryChnl + dTickSize;
    }

    return dPoints;
}

int checkExit(int iType, double dStopLoss, double dTrailStop, double dLow, double dHigh,
    char* szExitDate, char* szCurrDate, double dEntryPoints)
{
    // Determines whether we should exit the trade. Checks if we've
    // passed the exit date. Compares the closer of the trail stop
    // and stop loss with the high if we're selling short; and with
    // the low if we're selling long.

    double dCloser;

    //if we've passed the exit date
    if((strncmp(szCurrDate, szExitDate, DATE_LEN) > 0)) return FORCEDEXIT; //exit!
    else
    {
        //if short compare high to closer
        if(iType == SHORT)
        {
            //may exit due to stoploss
            if(dStopLoss < dTrailStop) 
                dCloser = dStopLoss;

            //may exit due to trailStop
            else 
            {
                //do not exit due to trailStop, cause we'd lose money
                if(dTrailStop > dEntryPoints)
                    return 0; //do not exit

                //otherwise, check as normal
                dCloser = dTrailStop;
            }

            if(dHigh > dCloser) return 1; //exit
            else return 0; //do not exit
        }

        //if long compare low to closer
        else
        {   //may exit due to stoploss
            if(dStopLoss > dTrailStop)
                dCloser = dStopLoss;

            //may exit due to trailStop
            else
            {
                //do note exit due to trailStop, cause we'd lose money
                if(dTrailStop < dEntryPoints)
                    return 0; //do not exit

                //otherwise, check as normal
                dCloser = dTrailStop;
            }

            if(dLow < dCloser) return 1; //exit
            else return 0; //do not exit
        }
    }

}

double computeExitPoints(int iType, double dTickSize, double dStopLoss, double dTrailStop,
    double dOpen, double dClose, int iExit)
{
    // Computes the exit points by comparing open to the closer of the
    // stop loss and the trail stop.

    double dPoints;
    double dCloser;

    //if a forced exit occurred, exit points is just close of today
    if(iExit == FORCEDEXIT)
        dPoints = dClose;

    //else compute exit points
    else
    {
        //compute short exit points
        if(iType == SHORT)
        {
            if(dStopLoss < dTrailStop) dCloser = dStopLoss;
            else dCloser = dTrailStop;

            if(dOpen > dCloser) dPoints = dOpen;
            else dPoints = dCloser + dTickSize;
        }

        //compute long exit points
        else
        {
            if(dStopLoss > dTrailStop) dCloser = dStopLoss;
            else dCloser = dTrailStop;

            if(dOpen < dCloser) dPoints = dOpen;
            else dPoints = dCloser - dTickSize;
        }
    }

    return dPoints;
}

double computeProfit(int iType, double dEntryPoints, double dExitPoints, double dTickVal, double dTickSize)
{
    // Computes the total profit earned.

    if(iType == LONG)
        return (dExitPoints - dEntryPoints) * dTickVal/dTickSize;
    else
        return (dEntryPoints - dExitPoints) * dTickVal/dTickSize;
}
