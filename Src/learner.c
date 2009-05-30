/*!
  \file    learner.c
  \brief   Helper functions shared by the learning functions
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#include <stdio.h>
#include <stdlib.h>
#include "tradeSystem.h"


/* ------------------------------- utility prototypes ----------------------- */

int Randomize(int* piCur, int iMin, int iMax, int distType);

int DateToDays(char* szDate);

void DaysToDate(char* szDate, int iDay);

/* ------------------------------- utility functions ------------------------ */

int Randomize(int* piCur, int iMin, int iMax, int distType)
{
    do
    {
        switch(distType)
        {
        case UNIFORM:
            *piCur = (int)(random()%(iMax-iMin)+iMin); //equal chance of jumping anywhere
            break;
        case NORMAL:
            *piCur = (int)(rnorm()*iMax/STDDEV+*piCur); //an STDDEV of 90 days
            break;
        case CONSTANT:
            *piCur = random()%2 > 0 ? 1 : -1 + *piCur; // move only by one
            break;
        default:
            FPRINTE(stderr,"Incorrect distType (%d)\n",distType);
            return ERRVAL;
        }
    }
    while (*piCur < iMin || *piCur > iMax);
    return 0;
}

void Neighbor(int* piCurEntryWindow, int* piCurTrailStopWindow,
              int* piCurStopLossWindow, char* szCurEntryDate,
              char* szCurNoEntryDate, char* szCurExitDate,
              int iArgsToRandomize, int distType)
{
    int i;
    int* pTemp;
    int r;
    int piCurEntryDate[] = {DateToDays(szCurEntryDate)};
    int piCurNoEntryDate[] = {DateToDays(szCurNoEntryDate)};
    int piCurExitDate[] = {DateToDays(szCurExitDate)};
    int* ppiArgs[] = {
        piCurEntryWindow, piCurTrailStopWindow, piCurStopLossWindow,
        piCurEntryDate,   piCurNoEntryDate,     piCurExitDate};
    int iMax;

    for (i = 0; i < iArgsToRandomize; i++)
    {
        r = (int)random()%(6-i)+i;
        pTemp = ppiArgs[r];
        ppiArgs[r] = ppiArgs[i];
        ppiArgs[i] = pTemp;

        if (ppiArgs[i] == piCurEntryWindow ||
            ppiArgs[i] == piCurTrailStopWindow ||
            ppiArgs[i] == piCurStopLossWindow)
            iMax = MAX_WINDOW_SIZE;
        else
            iMax = MAX_DATE_SIZE;

        Randomize(ppiArgs[i],1,iMax,distType);
    }

    DaysToDate(szCurEntryDate,*piCurEntryDate);
    DaysToDate(szCurNoEntryDate,*piCurNoEntryDate);
    DaysToDate(szCurExitDate,*piCurExitDate);
}

const int iDaysInMonths[] = {31,28,31,30,31,30,31,31,30,31,30,31};

int DateToDays(char* szDate)
{
    int iDate = atoi(szDate);
    int iDay = iDate % 100;
    int iMonth = iDate/100 - 1;

    while (iMonth > 0)
    {
        iDay += iDaysInMonths[iMonth--];
    }

    return iDay;
}


void DaysToDate(char* szDate, int iDay)
{
    int iMonth = 0;

    while (iDay > iDaysInMonths[iMonth])
    {
        iDay -= iDaysInMonths[iMonth++];
    }

    sprintf(szDate,"%.2d%.2d",++iMonth,iDay);
}
