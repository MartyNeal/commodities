#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tradeSystem.h"


/* ------------------------------- utility prototypes ----------------------- */

double StayCool(int iCurrentIteration, int iTotalIterations);

double StayHot(int iCurrentIteration, int iTotalIterations);

double LinearCoolingFunction(int iCurrentIteration, int iTotalIterations);

int Randomize(int* piCur, int iMin, int iMax, int distType);

int DateToDays(char* szDate);

void DaysToDate(char* szDate, int iDay);

/* ----------------------------- Simulated Annealing ------------------------ */

double SimulatedAnnealing(int iIterations, char* szCommodName, int iArgsToRandomize, int distType)
{
    int iCurEntryWindow;
    int iCurTrailStopWindow;
    int iCurStopLossWindow;
    char szCurEntryDate[5];
    char szCurNoEntryDate[5];
    char szCurExitDate[5];
    double dCurProfit;

    int iNextEntryWindow;
    int iNextTrailStopWindow;
    int iNextStopLossWindow;
    char szNextEntryDate[5];
    char szNextNoEntryDate[5];
    char szNextExitDate[5];
    double dNextProfit;

    int iBestEntryWindow;
    int iBestTrailStopWindow;
    int iBestStopLossWindow;
    char szBestEntryDate[5];
    char szBestNoEntryDate[5];
    char szBestExitDate[5];
    double dBestProfit;

    int i;
    double (*CoolingFunction)(int, int) = &LinearCoolingFunction;
    double dDeltaE;
    double dDeltaM;
    double mean;
    double var;
    double M2;
    double p;
    double r;

    //set current and best to all uniformly random parameters
    Neighbor(&iCurEntryWindow, &iCurTrailStopWindow, &iCurStopLossWindow,
             szCurEntryDate, szCurNoEntryDate, szCurExitDate, 6,
             UNIFORM);
    dCurProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                 iCurEntryWindow, iCurTrailStopWindow,
                                 iCurStopLossWindow, szCurEntryDate,
                                 szCurNoEntryDate, szCurExitDate);

    iBestEntryWindow = iCurEntryWindow;
    iBestTrailStopWindow = iCurTrailStopWindow;
    iBestStopLossWindow = iCurStopLossWindow;
    strcpy(szBestEntryDate, szCurEntryDate);
    strcpy(szBestNoEntryDate, szCurNoEntryDate);
    strcpy(szBestExitDate, szCurExitDate);
    dBestProfit = dCurProfit;

    mean = 0;
    M2 = 0;
    //    printf("first dProfit=%d\n",(int)dCurProfit);

    for (i = 0; i < iIterations; i++)
    {
        //calculate a successor for current
        iNextEntryWindow = iCurEntryWindow;
        iNextTrailStopWindow = iCurTrailStopWindow;
        iNextStopLossWindow = iCurStopLossWindow;
        strcpy(szNextEntryDate,szCurEntryDate);
        strcpy(szNextNoEntryDate,szCurNoEntryDate);
        strcpy(szNextExitDate,szCurExitDate);
        Neighbor(&iNextEntryWindow, &iNextTrailStopWindow, &iNextStopLossWindow,
                 szNextEntryDate, szNextNoEntryDate, szNextExitDate,
                 iArgsToRandomize, distType);
        dNextProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                      iNextEntryWindow, iNextTrailStopWindow,
                                      iNextStopLossWindow, szNextEntryDate,
                                      szNextNoEntryDate, szNextExitDate);
        //        printf("Next dProfit=%d\n",(int)dNextProfit);

        //update the mean and variance using newly sampled instance
        dDeltaE = dNextProfit - dCurProfit;
        dDeltaM = dDeltaE - mean;
        mean = mean + dDeltaM/(i+1);
        M2 = M2 + dDeltaM*(dDeltaE - mean);
        var = M2/(i+1);
        p = pnorm(dDeltaE,mean,sqrt(var));

        //check to see if the successor is good enough
        r = (double)random()/RAND_MAX;

        /*
        printf("dDeltaE = %d\n",(int)dDeltaE);
        printf("mean = %d\n",(int)mean);
        printf("sd = %d\n",(int)sqrt(var));
        printf("p = %.3lf\n",p);
        printf("T = %.3lf\n",CoolingFunction(i,iIterations));
        printf("random() = %.3lf\n",r);
        */
        if (dDeltaE >= 0 ||
            CoolingFunction(i,iIterations)*pnorm(dDeltaE,mean,sqrt(var)) > r)
        {
            //            if(dDeltaE < 0)
            //                printf("we took it\n");
            iCurEntryWindow = iNextEntryWindow;
            iCurTrailStopWindow = iNextTrailStopWindow;
            iCurStopLossWindow = iNextStopLossWindow;
            strcpy(szCurEntryDate,szNextEntryDate);
            strcpy(szCurNoEntryDate,szNextNoEntryDate);
            strcpy(szCurExitDate,szNextExitDate);
            dCurProfit = dNextProfit;
        }

        //if its the best so far, save it
        if (dCurProfit > dBestProfit)
        {
            iBestEntryWindow = iCurEntryWindow;
            iBestTrailStopWindow = iCurTrailStopWindow;
            iBestStopLossWindow = iCurStopLossWindow;
            strcpy(szCurEntryDate,szBestEntryDate);
            strcpy(szCurNoEntryDate,szBestNoEntryDate);
            strcpy(szCurExitDate,szBestExitDate);
            dBestProfit = dCurProfit;
        }
    }
    dCurProfit = tradeSystemData(szCommodName,TEST_YEARS,
                                 iCurEntryWindow, iCurTrailStopWindow, iCurStopLossWindow,
                                 szCurEntryDate, szCurNoEntryDate, szCurExitDate);

    return dBestProfit;
}


/* -------------------------------- utility functions ---------------------------- */

double StayCool(int iCurrentIteration, int iTotalIterations)
{
    return 0;
}


//does a random walk
double StayHot(int iCurrentIteration, int iTotalIterations)
{
    return 1;
}


double LinearCoolingFunction(int iCurrentIteration, int iTotalIterations)
{
    return 1 - (double)iCurrentIteration/iTotalIterations;
}


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
            fprintf(stderr,"ERROR:  incorrect distType (%d)\n",distType);
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
