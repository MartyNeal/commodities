#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tradeSystem.h"


/* ------------------------------- utility prototypes ----------------------- */

double StayCool(int iCurrentIteration, int iTotalIterations);

double StayHot(int iCurrentIteration, int iTotalIterations);

double LinearCoolingFunction(int iCurrentIteration, int iTotalIterations);


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
